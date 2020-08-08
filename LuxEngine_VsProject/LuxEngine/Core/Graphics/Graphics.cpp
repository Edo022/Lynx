

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
		TryVk(glfwCreateWindowSurface(instance, wnd::window, nullptr, &surface)) printError("Failed to create window surface");
		Normal printf("    Searching for physical devices...    ");		dvc::deviceGetPhysical( );											NewLine;
		cmd::createGraphicsCommandPool( );
		Normal printf("    Creating VK swapchain...             ");		swapchain::swapchainCreate( );					SuccessNoNl printf("ok");

		luxDebug(graphicsCreateDebugMessenger( ));
		graphicsCreateSyncObjs( );
	}


	luxDebug(void graphicsCreateDebugMessenger( ) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		TryVk(debug::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) printError("Failed to set up debug messenger");
	});


	void graphicsCreateSyncObjs( ) {
		drawFrameImageAquiredSemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameObjectsRenderedSemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameCopySemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameClearSemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameImageRenderedFence.resize(out::renderMaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{ };
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{ };
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {
			if(vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameImageAquiredSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameObjectsRenderedSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameCopySemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameClearSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(dvc::graphics.LD, &fenceInfo, nullptr, &drawFrameImageRenderedFence[i]) != VK_SUCCESS
				){
				printError("Failed to create vulkan sync objects");
			}
		}
	}






	//TODO multithreaded submit and command creation
	void graphicsDrawFrame( ) {
		if(c::shaders::CShaders.usedSize( ) <= 1) return;
		vkWaitForFences(dvc::graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], false, INT_MAX);
		redraw:




		if(out::renderFramebufferResized) {
			out::renderFramebufferResized = false;
			swapchain::swapchainRecreate(true);
			goto redraw;
		}

		uint32 imageIndex;
		{ //Acquire swapchain image
			switch(vkAcquireNextImageKHR(dvc::graphics.LD, swapchain::swapchain, /*1000*1000*5*/INT_MAX /*5s*/, drawFrameImageAquiredSemaphore[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: swapchain::swapchainRecreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
		{ //Update render result submitting the command buffers to the compute queues
			c::shaders::addShaderFence.startFirst( );
			c::shaders::CShadersCBs.resize(c::shaders::CShaders.usedSize( ));
			for(uint32 i = 0; i < c::shaders::CShaders.size( ); ++i) {
				if(c::shaders::CShaders.isValid(i)) c::shaders::CShadersCBs[i] = c::shaders::CShaders[i].commandBuffers[0];
			}
			c::shaders::addShaderFence.endFirst( );

			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.pNext{ nullptr },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &drawFrameImageAquiredSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			submitInfo.commandBufferCount = c::shaders::CShadersCBs.size( );
			submitInfo.pCommandBuffers = c::shaders::CShadersCBs.begin( );
			TryVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr)) printError("Failed to submit graphics command buffer");
		}




		{ //Convert and clear
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.pCommandBuffers{ &c::shaders::CShaders[0].commandBuffers[0] },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			TryVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr)) printError("Failed to submit graphics command buffer");
		}




		{ //Copy
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			submitInfo.pCommandBuffers = &c::copyCommandBuffers[imageIndex];

			vkResetFences(dvc::graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
			TryVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame])) printError("Failed to submit graphics command buffer");
		}




		{ //Present
			static VkPresentInfoKHR presentInfo{
				.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount{ 1 },
				.swapchainCount{ 1 },
				.pSwapchains{ &swapchain::swapchain },
			};
			presentInfo.pWaitSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			presentInfo.pImageIndices = &imageIndex;

			switch(vkQueuePresentKHR(dvc::graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					swapchain::swapchainRecreate(false);
					vkDeviceWaitIdle(dvc::graphics.LD);
					break;
				}
				default:  printError("Failed to present swapchain image");
			}

		}

		//Update frame number and flush the window data
		renderCurrentFrame = (renderCurrentFrame + 1) % (out::renderMaxFramesInFlight);
		glfwSwapBuffers(wnd::window);
	}








	void graphicsCleanup( ) {
		swapchain::swapchainCleanup( );																//Clear swapchain components
		vkDestroyCommandPool(dvc::graphics.LD, cmd::singleTimeCommandPool, nullptr);					//Destroy graphics command pool

		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {								//Destroy sync objects
			vkDestroySemaphore(dvc::graphics.LD, drawFrameImageAquiredSemaphore[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, drawFrameObjectsRenderedSemaphore[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, drawFrameCopySemaphore[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, drawFrameClearSemaphore[i], nullptr);
			vkDestroyFence(dvc::graphics.LD, drawFrameImageRenderedFence[i], nullptr);
		}


		if(dvc::graphics.PD.properties.deviceID != dvc::compute.PD.properties.deviceID) vkDestroyDevice(dvc::graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
		vkDestroyDevice(dvc::compute.LD, nullptr);																			//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);											//Destroy all the secondary devices

		luxDebug(debug::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));						//Destroy the debug messenger if present
		vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
	}








	// Other ------------------------------------------------------------------------------------------------------------------------------------//










	VkFormat graphicsFindSupportedFormat(const Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(dvc::graphics.PD.device, format, &props);

			if((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
				return format;
			}
		}
		printError("Failed to find a supported format");
	}




	//Returns the index of the memory with the specified type and properties
	uint32 graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(dvc::graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		printError("Failed to find suitable memory type");
	}
}