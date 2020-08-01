

#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/CShader_t.h"
#include "LuxEngine/Core/Graphics/Graphics.h"







namespace lux::core::g{
	Array<VkSemaphore>	drawFrameImageAquiredSemaphore;
	Array<VkSemaphore>	drawFrameObjectsRenderedSemaphore;
	Array<VkSemaphore>	drawFrameCopySemaphore;
	Array<VkSemaphore>	drawFrameClearSemaphore;
	Array<VkFence>		drawFrameImageRenderedFence;
	int32				renderCurrentFrame = 0;








	void graphicsInit(const bool vUseVSync, const float vFOV) {
		lux::getEngine( ).useVSync = vUseVSync;
		lux::getEngine( ).FOV = vFOV;

		luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;

		//Initialize vulkan
		TryVk(glfwCreateWindowSurface(lux::getEngine( ).instance, lux::core::g::window, nullptr, &lux::getEngine( ).surface)) Exit("Failed to create window surface");
		Normal printf("    Searching for physical devices...    ");		lux::getEngine( ).deviceGetPhysical( );											NewLine;
		lux::getEngine( ).createGraphicsCommandPool( );
		Normal printf("    Creating VK swapchain...             ");		lux::getEngine( ).swapchainCreate( );					SuccessNoNl printf("ok");

		luxDebug(graphicsCreateDebugMessenger( ));
		graphicsCreateSyncObjs( );
	}


	luxDebug(void lux::core::g::graphicsCreateDebugMessenger( ) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		lux::_engine::populateDebugMessengerCreateInfo(createInfo);
		TryVk(lux::_engine::CreateDebugUtilsMessengerEXT(lux::getEngine( ).instance, &createInfo, nullptr, &lux::getEngine( ).debugMessenger)) Exit("Failed to set up debug messenger");
	});


	void graphicsCreateSyncObjs( ) {
		lux::core::g::drawFrameImageAquiredSemaphore.resize(lux::getEngine( ).renderMaxFramesInFlight);
		lux::core::g::drawFrameObjectsRenderedSemaphore.resize(lux::getEngine( ).renderMaxFramesInFlight);
		lux::core::g::drawFrameCopySemaphore.resize(lux::getEngine( ).renderMaxFramesInFlight);
		lux::core::g::drawFrameClearSemaphore.resize(lux::getEngine( ).renderMaxFramesInFlight);
		lux::core::g::drawFrameImageRenderedFence.resize(lux::getEngine( ).renderMaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{ };
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{ };
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(int32 i = 0; i < lux::getEngine( ).renderMaxFramesInFlight; ++i) {
			if(vkCreateSemaphore(lux::getEngine( ).graphics.LD, &semaphoreInfo, nullptr, &lux::core::g::drawFrameImageAquiredSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(lux::getEngine( ).graphics.LD, &semaphoreInfo, nullptr, &lux::core::g::drawFrameObjectsRenderedSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(lux::getEngine( ).graphics.LD, &semaphoreInfo, nullptr, &lux::core::g::drawFrameCopySemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(lux::getEngine( ).graphics.LD, &semaphoreInfo, nullptr, &lux::core::g::drawFrameClearSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(lux::getEngine( ).graphics.LD, &fenceInfo, nullptr, &lux::core::g::drawFrameImageRenderedFence[i]) != VK_SUCCESS
				){
				Exit("Failed to create vulkan sync objects");
			}
		}
	}






	//TODO multithreaded submit and command creation
	void graphicsDrawFrame( ) {
		if(lux::core::c::CShaders.usedSize( ) <= 1) return;
		vkWaitForFences(lux::getEngine( ).graphics.LD, 1, &lux::core::g::drawFrameImageRenderedFence[lux::core::g::renderCurrentFrame], false, INT_MAX);
		redraw:




		if(lux::core::g::renderFramebufferResized) {
			lux::core::g::renderFramebufferResized = false;
			lux::getEngine( ).swapchainRecreate(true);
			goto redraw;
		}

		uint32 imageIndex;
		{ //Acquire swapchain image
			switch(vkAcquireNextImageKHR(lux::getEngine( ).graphics.LD, lux::getEngine( ).swapchain, /*1000*1000*5*/INT_MAX /*5s*/, lux::core::g::drawFrameImageAquiredSemaphore[lux::core::g::renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: lux::getEngine( ).swapchainRecreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
		{ //Update render result submitting the command buffers to the compute queues
			lux::core::c::addShaderFence.startFirst( );
			for(uint32 i = 0; i < lux::core::c::CShaders.size( ); ++i) {
				lux::core::c::CShadersCBs.resize(lux::core::c::CShaders.usedSize( ));
				if(lux::core::c::CShaders.isValid(i)) lux::core::c::CShadersCBs[i] = lux::core::c::CShaders[i].commandBuffers[0];
			}
			lux::core::c::addShaderFence.endFirst( );

			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.pNext{ nullptr },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &lux::core::g::drawFrameImageAquiredSemaphore[lux::core::g::renderCurrentFrame];
			submitInfo.pSignalSemaphores = &lux::core::g::drawFrameObjectsRenderedSemaphore[lux::core::g::renderCurrentFrame];
			submitInfo.commandBufferCount = lux::core::c::CShadersCBs.size( );
			submitInfo.pCommandBuffers = lux::core::c::CShadersCBs.begin( );
			TryVk(vkQueueSubmit(lux::getEngine( ).graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
		}




		{ //Convert and clear
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.pCommandBuffers{ &lux::core::c::CShaders[0].commandBuffers[0] },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &lux::core::g::drawFrameObjectsRenderedSemaphore[lux::core::g::renderCurrentFrame];
			submitInfo.pSignalSemaphores = &lux::core::g::drawFrameClearSemaphore[lux::core::g::renderCurrentFrame];
			TryVk(vkQueueSubmit(lux::getEngine( ).graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
		}




		{ //Copy
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &lux::core::g::drawFrameClearSemaphore[lux::core::g::renderCurrentFrame];
			submitInfo.pSignalSemaphores = &lux::core::g::drawFrameCopySemaphore[lux::core::g::renderCurrentFrame];
			submitInfo.pCommandBuffers = &lux::core::c::copyCommandBuffers[imageIndex];

			vkResetFences(lux::getEngine( ).graphics.LD, 1, &lux::core::g::drawFrameImageRenderedFence[lux::core::g::renderCurrentFrame]);
			TryVk(vkQueueSubmit(lux::getEngine( ).graphics.graphicsQueue, 1, &submitInfo, lux::core::g::drawFrameImageRenderedFence[lux::core::g::renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
		}




		{ //Present
			static VkPresentInfoKHR presentInfo{
				.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount{ 1 },
				.swapchainCount{ 1 },
				.pSwapchains{ &lux::getEngine( ).swapchain },
			};
			presentInfo.pWaitSemaphores = &lux::core::g::drawFrameCopySemaphore[lux::core::g::renderCurrentFrame];
			presentInfo.pImageIndices = &imageIndex;

			switch(vkQueuePresentKHR(lux::getEngine( ).graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					lux::getEngine( ).swapchainRecreate(false);
					vkDeviceWaitIdle(lux::getEngine( ).graphics.LD);
					break;
				}
				default:  Exit("Failed to present swapchain image");
			}

		}

		//Update frame number and flush the window data
		lux::core::g::renderCurrentFrame = (lux::core::g::renderCurrentFrame + 1) % (lux::getEngine( ).renderMaxFramesInFlight);
		glfwSwapBuffers(lux::core::g::window);
	}








	void graphicsCleanup( ) {
		lux::getEngine( ).swapchainCleanup( );																//Clear swapchain components
		vkDestroyCommandPool(lux::getEngine( ).graphics.LD, lux::getEngine( ).singleTimeCommandPool, nullptr);					//Destroy graphics command pool

		for(int32 i = 0; i < lux::getEngine( ).renderMaxFramesInFlight; ++i) {								//Destroy sync objects
			vkDestroySemaphore(lux::getEngine( ).graphics.LD, lux::core::g::drawFrameImageAquiredSemaphore[i], nullptr);
			vkDestroySemaphore(lux::getEngine( ).graphics.LD, lux::core::g::drawFrameObjectsRenderedSemaphore[i], nullptr);
			vkDestroySemaphore(lux::getEngine( ).graphics.LD, lux::core::g::drawFrameCopySemaphore[i], nullptr);
			vkDestroySemaphore(lux::getEngine( ).graphics.LD, lux::core::g::drawFrameClearSemaphore[i], nullptr);
			vkDestroyFence(lux::getEngine( ).graphics.LD, lux::core::g::drawFrameImageRenderedFence[i], nullptr);
		}


		if(lux::getEngine( ).graphics.PD.properties.deviceID != lux::getEngine( ).compute.PD.properties.deviceID) vkDestroyDevice(lux::getEngine( ).graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
		vkDestroyDevice(lux::getEngine( ).compute.LD, nullptr);																			//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);											//Destroy all the secondary devices

		luxDebug(lux::_engine::DestroyDebugUtilsMessengerEXT(lux::getEngine( ).instance, lux::getEngine( ).debugMessenger, nullptr));						//Destroy the debug messenger if present
		vkDestroySurfaceKHR(lux::getEngine( ).instance, lux::getEngine( ).surface, nullptr);																//Destroy the vulkan surface
	}








	// Other ------------------------------------------------------------------------------------------------------------------------------------//










	VkFormat graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(lux::getEngine( ).graphics.PD.device, format, &props);

			if((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
				return format;
			}
		}
		Exit("Failed to find a supported format");
	}




	//Returns the index of the memory with the specified type and properties
	uint32 graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(lux::getEngine( ).graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		Exit("Failed to find suitable memory type");
	}
}