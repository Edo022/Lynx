#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include <climits>








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	alignCache RtArray<obj::Base*>  objUpdates2D;
	alignCache std::mutex              objUpdates2D_f;
	alignCache std::mutex graphicsQueueSubmit_m;
	alignCache std::mutex presentQueueSubmit_m;



	#ifdef LUX_DEBUG
	void createDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		dbg::checkVk(debug::CreateDebugUtilsMessengerEXT(dvc::instance, &createInfo, nullptr, &dvc::debugMessenger), "Failed to set up debug messenger");
	}
	#endif








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//








	//TODO multithreaded submit and command creation
	void drawFrame(Window& pWindow) {
		if(pWindow.swapchain.shaders.count() <= 1) return;
		vkWaitForFences(dvc::graphics.LD, 1, &pWindow.swapchain.f_imageRendered[pWindow.swapchain.renderCurrentFrame], false, INT_MAX);


		//Redraw frame if necessary
		redraw:
		if(pWindow.swapchain.renderFramebufferResized) {
			// pWindow.windowResizeFence.lock();	//FIXME probably useless
			pWindow.swapchain.renderFramebufferResized = false;
			// pWindow.windowResizeFence.unlock();	//FIXME probably useless
			// pWindow.swapchain.recreate(true);
			pWindow.swapchain.recreate();
			goto redraw;
		}


		//Acquire swapchain image
		uint32 imageIndex;
		{
			switch(vkAcquireNextImageKHR(dvc::graphics.LD, pWindow.swapchain.swapchain, INT_MAX, pWindow.swapchain.s_imageAcquired[pWindow.swapchain.renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) { //FIXME DONT DEPEND ON A WINDOW
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				// case VK_ERROR_OUT_OF_DATE_KHR: pWindow.swapchain.recreate(false);  return;
				case VK_ERROR_OUT_OF_DATE_KHR: pWindow.swapchain.recreate();  return;
				default: Failure printf("Failed to acquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		//Update render result submitting the command buffers to the compute queues
		static const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		{
			pWindow.addShaderFence.lock();
			pWindow.swapchain.shadersCBs.resize(pWindow.swapchain.shaders.count());
			for(uint32 i = 0; i < pWindow.swapchain.shaders.count(); ++i) {
				pWindow.swapchain.shadersCBs[i] = pWindow.swapchain.shaders[i].commandBuffers[0];
			}
			pWindow.addShaderFence.unlock();

			VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.pNext                = nullptr,

				.waitSemaphoreCount   = 1,
				.pWaitSemaphores      = &pWindow.swapchain.s_imageAcquired[pWindow.swapchain.renderCurrentFrame],
				.pWaitDstStageMask    = waitStages,

				.commandBufferCount = pWindow.swapchain.shadersCBs.count(),
				.pCommandBuffers    = pWindow.swapchain.shadersCBs.begin(),

				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &pWindow.swapchain.s_objectsRendered[pWindow.swapchain.renderCurrentFrame],
			};
			graphicsQueueSubmit_m.lock();
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
			graphicsQueueSubmit_m.unlock();
		}



		{ //Convert and clear shader
			VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount   = 1,
				.pWaitSemaphores      = &pWindow.swapchain.s_objectsRendered[pWindow.swapchain.renderCurrentFrame],
				.pWaitDstStageMask    = waitStages,
				.commandBufferCount   = 1,
				.pCommandBuffers      = &pWindow.swapchain.shaders[0].commandBuffers[0],
				.signalSemaphoreCount = 1,
				.pSignalSemaphores    = &pWindow.swapchain.s_clear[pWindow.swapchain.renderCurrentFrame]
			};



			graphicsQueueSubmit_m.lock();
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
			graphicsQueueSubmit_m.unlock();
		}




		{ //Copy shader
			VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount   = 1,
				.pWaitSemaphores      = &pWindow.swapchain.s_clear[pWindow.swapchain.renderCurrentFrame],
				.pWaitDstStageMask    = waitStages,
				.commandBufferCount   = 1,
				.pCommandBuffers      = &pWindow.copyCommandBuffers[imageIndex],
				.signalSemaphoreCount = 1,
				.pSignalSemaphores    = &pWindow.swapchain.s_copy[pWindow.swapchain.renderCurrentFrame]
			};

			vkResetFences(dvc::graphics.LD, 1, &pWindow.swapchain.f_imageRendered[pWindow.swapchain.renderCurrentFrame]);
			graphicsQueueSubmit_m.lock();
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, pWindow.swapchain.f_imageRendered[pWindow.swapchain.renderCurrentFrame]), "Failed to submit graphics command buffer");
			graphicsQueueSubmit_m.unlock();
		}




		{ //Present frame
			VkPresentInfoKHR presentInfo{
				.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = 1,
				.pWaitSemaphores    = &pWindow.swapchain.s_copy[pWindow.swapchain.renderCurrentFrame],
				.swapchainCount     = 1,
				.pSwapchains        = &pWindow.swapchain.swapchain,
				.pImageIndices      = &imageIndex
			};

			presentQueueSubmit_m.lock();
			switch(vkQueuePresentKHR(dvc::graphics.presentQueue, &presentInfo)) { //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
				case VK_SUCCESS:  break;
				//TODO maybe suboptimal can still be used
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					// pWindow.swapchain.recreate(false);
					pWindow.swapchain.recreate();
					presentQueueSubmit_m.unlock();
					goto redraw;
				}
				default: dbg::printError("Failed to present swapchain image");
			}
			presentQueueSubmit_m.unlock();

		}

		//Update frame number and flush the window data
		pWindow.swapchain.renderCurrentFrame = (pWindow.swapchain.renderCurrentFrame + 1) % lux::core::wnd::__renderMaxFramesInFlight;
		glfwSwapBuffers(pWindow.window);




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
			core::render::cmd::endSingleTimeCommands(cb); //FIXME USE LOCAL COMMAND BUFFER INSTEAD OF THE GLOBAL ONE
			objUpdates2D.clear();
			objUpdates2D_f.unlock();
		}
	}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








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