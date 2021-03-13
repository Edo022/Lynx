#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include <climits>








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	alignCache RtArray<obj::Base*> objUpdates2D;
	alignCache std::mutex          objUpdates2D_f;
	alignCache std::mutex graphicsQueueSubmit_m;
	alignCache std::mutex presentQueueSubmit_m;



	#ifdef LUX_DEBUG
	void createDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		dbg::checkVk(debug::CreateDebugUtilsMessengerEXT(dvc::instance, &createInfo, nullptr, &dvc::debugMessenger), "Failed to set up debug messenger");
	}
	#endif
}








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//








namespace lux{
	void Window::draw() {
		running = true;
		while(running) {
			sleep(0); //Prevent extra overhead when no object has to be rendered
			if(swapchain.shaders.count() <= 1) continue;
			vkWaitForFences(core::dvc::graphics.LD, 1, &swapchain.f_imageRendered[swapchain.renderCurrentFrame], false, INT_MAX);


			//Redraw frame if necessary
			redraw:
			if(swapchain.renderFramebufferResized) {
				swapchain.renderFramebufferResized = false;
				swapchain.recreate();
				goto redraw;
			}


			//Acquire swapchain image
			uint32 imageIndex;
			{
				switch(vkAcquireNextImageKHR(core::dvc::graphics.LD, swapchain.swapchain, INT_MAX, swapchain.s_imageAcquired[swapchain.renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) { //FIXME DONT DEPEND ON A WINDOW
					case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
					case VK_ERROR_OUT_OF_DATE_KHR: swapchain.recreate();  continue;
					default: Failure printf("Failed to acquire swapchain image");
				}
			}




			//TODO don't recreate the command buffer array every time
			//TODO use a staging buffer
			//Update render result submitting the command buffers to the compute queues
			const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
			addShaderFence.lock();
				swapchain.shadersCBs.resize(swapchain.shaders.count());
				for(uint32 i = 0; i < swapchain.shaders.count(); ++i) {
					swapchain.shadersCBs[i] = swapchain.shaders[i].commandBuffers[0];
				}
			addShaderFence.unlock();




			const VkSubmitInfo submitInfos[]{
				{ //Draw objects
					.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.waitSemaphoreCount   = 1,
					.pWaitSemaphores      = &swapchain.s_imageAcquired[swapchain.renderCurrentFrame],
					.pWaitDstStageMask    = waitStages,
					.commandBufferCount   = swapchain.shadersCBs.count(),
					.pCommandBuffers      = swapchain.shadersCBs.begin(),
					.signalSemaphoreCount = 1,
					.pSignalSemaphores    = &swapchain.s_objectsRendered[swapchain.renderCurrentFrame],
				},
				{ //Convert and clear shader
					.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.waitSemaphoreCount   = 1,
					.pWaitSemaphores      = &swapchain.s_objectsRendered[swapchain.renderCurrentFrame],
					.pWaitDstStageMask    = waitStages,
					.commandBufferCount   = 1,
					.pCommandBuffers      = &swapchain.shaders[0].commandBuffers[0],
					.signalSemaphoreCount = 1,
					.pSignalSemaphores    = &swapchain.s_clear[swapchain.renderCurrentFrame]
				},
				{ //Copy shader
					.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.waitSemaphoreCount   = 1,
					.pWaitSemaphores      = &swapchain.s_clear[swapchain.renderCurrentFrame],
					.pWaitDstStageMask    = waitStages,
					.commandBufferCount   = 1,
					.pCommandBuffers      = &copyCommandBuffers[imageIndex],
					.signalSemaphoreCount = 1,
					.pSignalSemaphores    = &swapchain.s_copy[swapchain.renderCurrentFrame]
				}
			};
			vkResetFences(core::dvc::graphics.LD, 1, &swapchain.f_imageRendered[swapchain.renderCurrentFrame]);
			core::render::graphicsQueueSubmit_m.lock();
				dbg::checkVk(vkQueueSubmit(core::dvc::graphics.graphicsQueue, 3, submitInfos, swapchain.f_imageRendered[swapchain.renderCurrentFrame]), "Failed to submit graphics command buffer");
			core::render::graphicsQueueSubmit_m.unlock();




			{ //Present frame
				const VkPresentInfoKHR presentInfo{
					.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
					.waitSemaphoreCount = 1,
					.pWaitSemaphores    = &swapchain.s_copy[swapchain.renderCurrentFrame],
					.swapchainCount     = 1,
					.pSwapchains        = &swapchain.swapchain,
					.pImageIndices      = &imageIndex
				};

				core::render::presentQueueSubmit_m.lock();
					auto presentResult = vkQueuePresentKHR(core::dvc::graphics.presentQueue, &presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
				core::render::presentQueueSubmit_m.unlock();
				switch(presentResult){
					case VK_SUCCESS:  break;
					case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: { //TODO maybe suboptimal can still be used
						swapchain.recreate();
						goto redraw;
					}
					default: dbg::printError("Failed to present swapchain image");
				}

			}

			//Update frame number and flush the window data
			swapchain.renderCurrentFrame = (swapchain.renderCurrentFrame + 1) % lux::core::wnd::__renderMaxFramesInFlight;
			glfwSwapBuffers(window);




			//TODO parallelize work from a secondary render thread
			//Fix objects update requests
			if(core::render::objUpdates2D.count() > 0) {
				core::render::objUpdates2D_f.lock();
				VkCommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
				for(uint32 i = 0; i < core::render::objUpdates2D.count(); i++) {
					core::render::objUpdates2D[i]->render.updated = true;
					vkCmdUpdateBuffer(
						cb,
						core::render::objUpdates2D[i]->render.localData.cell->csc.buffer,
						core::render::objUpdates2D[i]->render.localData.cell->localOffset,
						core::render::objUpdates2D[i]->cellSize,
						(void*)core::render::objUpdates2D[i]->render.data
					);
				}
				core::render::cmd::endSingleTimeCommands(cb); //FIXME USE LOCAL COMMAND BUFFER INSTEAD OF THE GLOBAL ONE
				core::render::objUpdates2D.clear();
				core::render::objUpdates2D_f.unlock();
			}
		}
	}
}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	void cleanup() {
		vkDestroyCommandPool(dvc::graphics.LD, cmd::singleTimeCommandPool, nullptr);	//Destroy graphics command pool

		//If the compute and the graphics devices are not the same, destroy the graphics device
		if(dvc::graphics.PD.properties.deviceID != dvc::compute.PD.properties.deviceID) vkDestroyDevice(dvc::graphics.LD, nullptr);
		vkDestroyDevice(dvc::compute.LD, nullptr);													//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);						//Destroy all the secondary devices

		_dbg(debug::DestroyDebugUtilsMessengerEXT(dvc::instance, dvc::debugMessenger, nullptr));	//Destroy the debug messenger if present
	}








// Other ------------------------------------------------------------------------------------------------------------------------------------//








	VkFormat findSupportedFormat(const RtArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(dvc::graphics.PD.device, format, &props);

			if(( vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == VK_IMAGE_TILING_LINEAR  && (props.linearTilingFeatures  & vFeatures) == vFeatures)) {
				return format;
			}
		}
		dbg::printError("Failed to find a supported format");
		return VK_FORMAT_UNDEFINED;
	}




	//Returns the index of the memory with the specified type and properties
	uint32 findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(dvc::graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		dbg::printError("Failed to find suitable memory type");
		return -1;
	}
}