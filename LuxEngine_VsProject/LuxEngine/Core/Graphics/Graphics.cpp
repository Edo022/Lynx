

#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CShader_t.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Devices.h"







namespace lux::core::g{
	Array<VkSemaphore>	drawFrameImageAquiredSemaphore;
	Array<VkSemaphore>	drawFrameObjectsRenderedSemaphore;
	Array<VkSemaphore>	drawFrameCopySemaphore;
	Array<VkSemaphore>	drawFrameClearSemaphore;
	Array<VkFence>		drawFrameImageRenderedFence;
	int32				renderCurrentFrame = 0;








	void graphicsInit(const bool vUseVSync, const float vFOV) {
		useVSync = vUseVSync;
		FOV = vFOV;

		luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;

		//Initialize vulkan
		TryVk(glfwCreateWindowSurface(instance, g::window, nullptr, &surface)) Exit("Failed to create window surface");
		Normal printf("    Searching for physical devices...    ");		g::deviceGetPhysical( );											NewLine;
		g::createGraphicsCommandPool( );
		Normal printf("    Creating VK swapchain...             ");		g::swapchainCreate( );					SuccessNoNl printf("ok");

		luxDebug(graphicsCreateDebugMessenger( ));
		graphicsCreateSyncObjs( );
	}


	luxDebug(void g::graphicsCreateDebugMessenger( ) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		lux::core::debug::populateDebugMessengerCreateInfo(createInfo);
		TryVk(lux::core::debug::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Exit("Failed to set up debug messenger");
	});


	void graphicsCreateSyncObjs( ) {
		g::drawFrameImageAquiredSemaphore.resize(g::renderMaxFramesInFlight);
		g::drawFrameObjectsRenderedSemaphore.resize(g::renderMaxFramesInFlight);
		g::drawFrameCopySemaphore.resize(g::renderMaxFramesInFlight);
		g::drawFrameClearSemaphore.resize(g::renderMaxFramesInFlight);
		g::drawFrameImageRenderedFence.resize(g::renderMaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{ };
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{ };
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(int32 i = 0; i < g::renderMaxFramesInFlight; ++i) {
			if(vkCreateSemaphore(g::graphics.LD, &semaphoreInfo, nullptr, &g::drawFrameImageAquiredSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(g::graphics.LD, &semaphoreInfo, nullptr, &g::drawFrameObjectsRenderedSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(g::graphics.LD, &semaphoreInfo, nullptr, &g::drawFrameCopySemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(g::graphics.LD, &semaphoreInfo, nullptr, &g::drawFrameClearSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(g::graphics.LD, &fenceInfo, nullptr, &g::drawFrameImageRenderedFence[i]) != VK_SUCCESS
				){
				Exit("Failed to create vulkan sync objects");
			}
		}
	}






	//TODO multithreaded submit and command creation
	void graphicsDrawFrame( ) {
		if(c::CShaders.usedSize( ) <= 1) return;
		vkWaitForFences(g::graphics.LD, 1, &g::drawFrameImageRenderedFence[g::renderCurrentFrame], false, INT_MAX);
		redraw:




		if(g::renderFramebufferResized) {
			g::renderFramebufferResized = false;
			g::swapchainRecreate(true);
			goto redraw;
		}

		uint32 imageIndex;
		{ //Acquire swapchain image
			switch(vkAcquireNextImageKHR(g::graphics.LD, g::swapchain, /*1000*1000*5*/INT_MAX /*5s*/, g::drawFrameImageAquiredSemaphore[g::renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: g::swapchainRecreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
		{ //Update render result submitting the command buffers to the compute queues
			c::addShaderFence.startFirst( );
			for(uint32 i = 0; i < c::CShaders.size( ); ++i) {
				c::CShadersCBs.resize(c::CShaders.usedSize( ));
				if(c::CShaders.isValid(i)) c::CShadersCBs[i] = c::CShaders[i].commandBuffers[0];
			}
			c::addShaderFence.endFirst( );

			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.pNext{ nullptr },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &g::drawFrameImageAquiredSemaphore[g::renderCurrentFrame];
			submitInfo.pSignalSemaphores = &g::drawFrameObjectsRenderedSemaphore[g::renderCurrentFrame];
			submitInfo.commandBufferCount = c::CShadersCBs.size( );
			submitInfo.pCommandBuffers = c::CShadersCBs.begin( );
			TryVk(vkQueueSubmit(g::graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
		}




		{ //Convert and clear
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.pCommandBuffers{ &c::CShaders[0].commandBuffers[0] },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &g::drawFrameObjectsRenderedSemaphore[g::renderCurrentFrame];
			submitInfo.pSignalSemaphores = &g::drawFrameClearSemaphore[g::renderCurrentFrame];
			TryVk(vkQueueSubmit(g::graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
		}




		{ //Copy
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &g::drawFrameClearSemaphore[g::renderCurrentFrame];
			submitInfo.pSignalSemaphores = &g::drawFrameCopySemaphore[g::renderCurrentFrame];
			submitInfo.pCommandBuffers = &c::copyCommandBuffers[imageIndex];

			vkResetFences(g::graphics.LD, 1, &g::drawFrameImageRenderedFence[g::renderCurrentFrame]);
			TryVk(vkQueueSubmit(g::graphics.graphicsQueue, 1, &submitInfo, g::drawFrameImageRenderedFence[g::renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
		}




		{ //Present
			static VkPresentInfoKHR presentInfo{
				.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount{ 1 },
				.swapchainCount{ 1 },
				.pSwapchains{ &g::swapchain },
			};
			presentInfo.pWaitSemaphores = &g::drawFrameCopySemaphore[g::renderCurrentFrame];
			presentInfo.pImageIndices = &imageIndex;

			switch(vkQueuePresentKHR(g::graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					g::swapchainRecreate(false);
					vkDeviceWaitIdle(g::graphics.LD);
					break;
				}
				default:  Exit("Failed to present swapchain image");
			}

		}

		//Update frame number and flush the window data
		g::renderCurrentFrame = (g::renderCurrentFrame + 1) % (g::renderMaxFramesInFlight);
		glfwSwapBuffers(g::window);
	}








	void graphicsCleanup( ) {
		g::swapchainCleanup( );																//Clear swapchain components
		vkDestroyCommandPool(g::graphics.LD, g::singleTimeCommandPool, nullptr);					//Destroy graphics command pool

		for(int32 i = 0; i < g::renderMaxFramesInFlight; ++i) {								//Destroy sync objects
			vkDestroySemaphore(g::graphics.LD, g::drawFrameImageAquiredSemaphore[i], nullptr);
			vkDestroySemaphore(g::graphics.LD, g::drawFrameObjectsRenderedSemaphore[i], nullptr);
			vkDestroySemaphore(g::graphics.LD, g::drawFrameCopySemaphore[i], nullptr);
			vkDestroySemaphore(g::graphics.LD, g::drawFrameClearSemaphore[i], nullptr);
			vkDestroyFence(g::graphics.LD, g::drawFrameImageRenderedFence[i], nullptr);
		}


		if(g::graphics.PD.properties.deviceID != g::compute.PD.properties.deviceID) vkDestroyDevice(g::graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
		vkDestroyDevice(g::compute.LD, nullptr);																			//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);											//Destroy all the secondary devices

		luxDebug(lux::core::debug::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));						//Destroy the debug messenger if present
		vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
	}








	// Other ------------------------------------------------------------------------------------------------------------------------------------//










	VkFormat graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(g::graphics.PD.device, format, &props);

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
		vkGetPhysicalDeviceMemoryProperties(g::graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		Exit("Failed to find suitable memory type");
	}
}