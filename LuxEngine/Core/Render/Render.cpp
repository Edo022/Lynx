#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include <climits>








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	alignCache RtArray<obj::Base*>  objUpdates2D;
	alignCache FenceDE              objUpdates2D_f;




	#ifdef LUX_DEBUG
	void createDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		dbg::checkVk(debug::CreateDebugUtilsMessengerEXT(dvc::instance, &createInfo, nullptr, &dvc::debugMessenger), "Failed to set up debug messenger");
	}
	#endif








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//








	//TODO multithreaded submit and command creation
	void drawFrame(Window& window) {
		if(window.swapchain.shaders.count() <= 1) return;
		vkWaitForFences(dvc::graphics.LD, 1, &window.swapchain.f_imageRendered[window.swapchain.renderCurrentFrame], false, INT_MAX);


		//Redraw frame if necessary
		redraw:
		if(window.swapchain.renderFramebufferResized) {
			window.windowResizeFence.lock();
			window.swapchain.renderFramebufferResized = false;
			window.windowResizeFence.unlock();
			window.swapchain.recreate(true);
			goto redraw;
		}


		//Acquire swapchain image
		uint32 imageIndex;
		{
			switch(vkAcquireNextImageKHR(dvc::graphics.LD, window.swapchain.swapchain, INT_MAX, window.swapchain.s_imageAquired[window.swapchain.renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) { //FIXME DONT DEPEND ON A WINDOW
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: window.swapchain.recreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		//Update render result submitting the command buffers to the compute queues
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		{
			window.addShaderFence.lock();
			window.swapchain.shadersCBs.resize(window.swapchain.shaders.count());
			for(uint32 i = 0; i < window.swapchain.shaders.count(); ++i) {
				window.swapchain.shadersCBs[i] = window.swapchain.shaders[i].commandBuffers[0];
			}
			window.addShaderFence.unlock();

			VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.pNext                = nullptr,

				.waitSemaphoreCount   = 1,
				.pWaitSemaphores      = &window.swapchain.s_imageAquired[window.swapchain.renderCurrentFrame],
				.pWaitDstStageMask    = waitStages,

				.commandBufferCount = window.swapchain.shadersCBs.count(),
				.pCommandBuffers    = window.swapchain.shadersCBs.begin(),

				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &window.swapchain.s_objectsRendered[window.swapchain.renderCurrentFrame],
			};
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
		}



		{ //Convert and clear shader
			static VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount   = 1,
				.pWaitDstStageMask    = waitStages,
				.commandBufferCount   = 1,
				.signalSemaphoreCount = 1
			};
			submitInfo.pCommandBuffers = &window.swapchain.shaders[0].commandBuffers[0];
			submitInfo.pWaitSemaphores = &window.swapchain.s_objectsRendered[window.swapchain.renderCurrentFrame];
			submitInfo.pSignalSemaphores = &window.swapchain.s_clear[window.swapchain.renderCurrentFrame];
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
		}




		{ //Copy shader
			static VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount   = 1,
				.pWaitDstStageMask    = waitStages,
				.commandBufferCount   = 1,
				.signalSemaphoreCount = 1
			};
			submitInfo.pWaitSemaphores   = &window.swapchain.s_clear[window.swapchain.renderCurrentFrame];
			submitInfo.pSignalSemaphores = &window.swapchain.s_copy [window.swapchain.renderCurrentFrame];
			submitInfo.pCommandBuffers   = &window.copyCommandBuffers[imageIndex];

			vkResetFences(dvc::graphics.LD, 1, &window.swapchain.f_imageRendered[window.swapchain.renderCurrentFrame]);
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, window.swapchain.f_imageRendered[window.swapchain.renderCurrentFrame]), "Failed to submit graphics command buffer");
		}




		{ //Present frame
			static VkPresentInfoKHR presentInfo{
				.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = 1,
				.swapchainCount     = 1,
				.pSwapchains        = &window.swapchain.swapchain
			};
			presentInfo.pWaitSemaphores = &window.swapchain.s_copy[window.swapchain.renderCurrentFrame];
			presentInfo.pImageIndices   = &imageIndex;

			switch(vkQueuePresentKHR(dvc::graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				//TODO maybe suboptimal can still be used
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					window.swapchain.recreate(false);
					goto redraw;
				}
				default: dbg::printError("Failed to present swapchain image");
			}

		}

		//Update frame number and flush the window data
		window.swapchain.renderCurrentFrame = (window.swapchain.renderCurrentFrame + 1) % lux::core::wnd::__renderMaxFramesInFlight;
		glfwSwapBuffers(window.window);




		//TODO parallelize work from a secondary render thread
		//Fix objects update requests
		if(objUpdates2D.count() > 0) {
			objUpdates2D_f.startFirst();
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
			objUpdates2D_f.endFirst();
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