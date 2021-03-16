#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include <climits>








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	// alignCache RtArray<obj::Base*> objUpdates2D;
	// alignCache std::mutex          objUpdates2D_f;
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
			if(swp.shaders.count() <= 1) continue;
			vkWaitForFences(core::dvc::graphics.LD, 1, &swp.f_imageRendered[swp.curFrame], false, INT_MAX);


			//Redraw frame if necessary
			redraw:
			if(swp.renderFramebufferResized) {
				swp.renderFramebufferResized = false;
				swp.recreate();
				goto redraw;
			}


			//Acquire swapchain image
			uint32 imageIndex;
			{
				switch(vkAcquireNextImageKHR(core::dvc::graphics.LD, swp.swapchain, INT_MAX, swp.s_imageAcquired[swp.curFrame], VK_NULL_HANDLE, &imageIndex)) {
					case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
					case VK_ERROR_OUT_OF_DATE_KHR: swp.recreate();  continue;
					default: Failure printf("Failed to acquire swapchain image");
				}
			}




			//TODO don't recreate the command buffer array every time
			//TODO use a staging buffer
			//Update render result submitting the command buffers to the compute queues
			const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
			addShaderFence.lock();
				swp.shadersCBs.resize(swp.shaders.count());
				for(uint32 i = 0; i < swp.shaders.count(); ++i) {
					swp.shadersCBs[i] = swp.shaders[i].commandBuffers[0];
				}
			addShaderFence.unlock();




			const VkSubmitInfo submitInfos[]{
				{ //Draw objects
					.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.waitSemaphoreCount   = 1,
					.pWaitSemaphores      = &swp.s_imageAcquired[swp.curFrame],
					.pWaitDstStageMask    = waitStages,
					.commandBufferCount   = swp.shadersCBs.count(),
					.pCommandBuffers      = swp.shadersCBs.begin(),
					.signalSemaphoreCount = 1,
					.pSignalSemaphores    = &swp.s_objectsRendered[swp.curFrame],
				},
				{ //Convert and clear shader
					.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.waitSemaphoreCount   = 1,
					.pWaitSemaphores      = &swp.s_objectsRendered[swp.curFrame],
					.pWaitDstStageMask    = waitStages,
					.commandBufferCount   = 1,
					.pCommandBuffers      = &swp.shaders[0].commandBuffers[0],
					.signalSemaphoreCount = 1,
					.pSignalSemaphores    = &swp.s_clear[swp.curFrame]
				},
				{ //Copy shader
					.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.waitSemaphoreCount   = 1,
					.pWaitSemaphores      = &swp.s_clear[swp.curFrame],
					.pWaitDstStageMask    = waitStages,
					.commandBufferCount   = 1,
					.pCommandBuffers      = &copyCommandBuffers[imageIndex],
					.signalSemaphoreCount = 1,
					.pSignalSemaphores    = &swp.s_copy[swp.curFrame]
				}
			};
			vkResetFences(core::dvc::graphics.LD, 1, &swp.f_imageRendered[swp.curFrame]);
			core::render::graphicsQueueSubmit_m.lock();
				dbg::checkVk(vkQueueSubmit(core::dvc::graphics.graphicsQueue, 3, submitInfos, swp.f_imageRendered[swp.curFrame]), "Failed to submit graphics command buffer");
			core::render::graphicsQueueSubmit_m.unlock();




			{ //Present frame
				const VkPresentInfoKHR presentInfo{
					.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
					.waitSemaphoreCount = 1,
					.pWaitSemaphores    = &swp.s_copy[swp.curFrame],
					.swapchainCount     = 1,
					.pSwapchains        = &swp.swapchain,
					.pImageIndices      = &imageIndex
				};

				core::render::presentQueueSubmit_m.lock();
					auto presentResult = vkQueuePresentKHR(core::dvc::graphics.presentQueue, &presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
				core::render::presentQueueSubmit_m.unlock();
				switch(presentResult){
					case VK_SUCCESS:  break;
					case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: { //TODO maybe suboptimal can still be used
						swp.recreate();
						goto redraw;
					}
					default: dbg::printError("Failed to present swapchain image");
				}

			}

			//Update frame number and flush the window data
			swp.curFrame = (swp.curFrame + 1) % lux::core::wnd::__renderMaxFramesInFlight;
			glfwSwapBuffers(window);




			//TODO parallelize work from a secondary render thread
			//Fix objects update requests
			if(objUpdates2D.count() > 0) {
				objUpdates2D_f.lock();
				VkCommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
				for(uint32 i = 0; i < objUpdates2D.count(); i++) {
					objUpdates2D[i]->render.updated = true;
					vkCmdUpdateBuffer(
						cb,
						objUpdates2D[i]->render.localData.cell->csc.buffer,
						objUpdates2D[i]->render.localData.cell->localOffset,
						objUpdates2D[i]->cellSize,
						(void*)objUpdates2D[i]->render.data
					);
				}
				core::render::cmd::endSingleTimeCommands(cb);
				objUpdates2D.clear();
				objUpdates2D_f.unlock();
			}
			//FIXME ADD COPY FROM RAM FUNCTTION TO VRAM ALLOCATIONS
			if(glfwWindowShouldClose(window)) return;
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
		return (uint32)-1;
	}
}