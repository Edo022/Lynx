

#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/CShader_t.h"





void Engine::graphicsInit(const bool vUseVSync, const float vFOV) {
	useVSync = vUseVSync;
	FOV = vFOV;

	luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;

	//Initialize vulkan
	TryVk(glfwCreateWindowSurface(instance, window, nullptr, &surface)) Exit("Failed to create window surface");
	Normal printf("    Searching for physical devices...    ");		deviceGetPhysical( );											NewLine;
	createGraphicsCommandPool( );
	Normal printf("    Creating VK swapchain...             ");		swapchainCreate( );					SuccessNoNl printf("ok");

	luxDebug(graphicsCreateDebugMessenger( ));
	graphicsCreateSyncObjs( );
}


luxDebug(void Engine::graphicsCreateDebugMessenger( ) {
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	lux::_engine::populateDebugMessengerCreateInfo(createInfo);
	TryVk(lux::_engine::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Exit("Failed to set up debug messenger");
})


void Engine::graphicsCreateSyncObjs( ) {
	drawFrameImageAquiredSemaphore.resize(renderMaxFramesInFlight);
	drawFrameObjectsRenderedSemaphore.resize(renderMaxFramesInFlight);
	drawFrameCopySemaphore.resize(renderMaxFramesInFlight);
	drawFrameClearSemaphore.resize(renderMaxFramesInFlight);
	drawFrameImageRenderedFence.resize(renderMaxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{ };
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{ };
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(int32 i = 0; i < renderMaxFramesInFlight; ++i) {
		if(vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameImageAquiredSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameObjectsRenderedSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameCopySemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameClearSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(graphics.LD, &fenceInfo, nullptr, &drawFrameImageRenderedFence[i]) != VK_SUCCESS
			){
			Exit("Failed to create vulkan sync objects");
		}
	}
}






//TODO multithreaded submit and command creation
void Engine::graphicsDrawFrame( ) {
	if(lux::core::c::CShaders.usedSize( ) <= 1) return;
	vkWaitForFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], false, INT_MAX);
	redraw:




	if(renderFramebufferResized) {
		renderFramebufferResized = false;
		swapchainRecreate(true);
		goto redraw;
	}

	uint32 imageIndex;
	{ //Acquire swapchain image
		switch(vkAcquireNextImageKHR(graphics.LD, swapchain, /*1000*1000*5*/INT_MAX /*5s*/, drawFrameImageAquiredSemaphore[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
			case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
			case VK_ERROR_OUT_OF_DATE_KHR: swapchainRecreate(false);  return;
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
		submitInfo.pWaitSemaphores = &drawFrameImageAquiredSemaphore[renderCurrentFrame];
		submitInfo.pSignalSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
		submitInfo.commandBufferCount = lux::core::c::CShadersCBs.size( );
		submitInfo.pCommandBuffers = lux::core::c::CShadersCBs.begin( );
		TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
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
		submitInfo.pWaitSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
		submitInfo.pSignalSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
		TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
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
		submitInfo.pCommandBuffers = &lux::core::c::copyCommandBuffers[imageIndex];

		vkResetFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
		TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
	}




	{ //Present
		static VkPresentInfoKHR presentInfo{
			.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
			.waitSemaphoreCount{ 1 },
			.swapchainCount{ 1 },
			.pSwapchains{ &swapchain },
		};
		presentInfo.pWaitSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
		presentInfo.pImageIndices = &imageIndex;

		switch(vkQueuePresentKHR(graphics.presentQueue, &presentInfo)) {
			case VK_SUCCESS:  break;
			case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
				swapchainRecreate(false);
				vkDeviceWaitIdle(graphics.LD);
				break;
			}
			default:  Exit("Failed to present swapchain image");
		}

	}

	//Update frame number and flush the window data
	renderCurrentFrame = (renderCurrentFrame + 1) % (renderMaxFramesInFlight);
	glfwSwapBuffers(window);
}








void Engine::framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
	lux::getEngine( ).windowResizeFence.startSecond( );  //from the last call of this function
	lux::getEngine( ).renderFramebufferResized = true;
	lux::getEngine( ).windowResizeFence.endSecond( );
}




void Engine::graphicsCleanup( ) {
	swapchainCleanup( );																//Clear swapchain components
	vkDestroyCommandPool(graphics.LD, singleTimeCommandPool, nullptr);					//Destroy graphics command pool

	for(int32 i = 0; i < renderMaxFramesInFlight; ++i) {								//Destroy sync objects
		vkDestroySemaphore(graphics.LD, drawFrameImageAquiredSemaphore[i], nullptr);
		vkDestroySemaphore(graphics.LD, drawFrameObjectsRenderedSemaphore[i], nullptr);
		vkDestroySemaphore(graphics.LD, drawFrameCopySemaphore[i], nullptr);
		vkDestroySemaphore(graphics.LD, drawFrameClearSemaphore[i], nullptr);
		vkDestroyFence(graphics.LD, drawFrameImageRenderedFence[i], nullptr);
	}


	if(graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
	vkDestroyDevice(compute.LD, nullptr);																			//Destroy the compute device
	//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);											//Destroy all the secondary devices

	luxDebug(lux::_engine::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));						//Destroy the debug messenger if present
	vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
}








// Other ------------------------------------------------------------------------------------------------------------------------------------//








VkFormat Engine::graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
	for(VkFormat format : *pCandidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

		if((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
			(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
			return format;
		}
	}
	Exit("Failed to find a supported format");
}




//Returns the index of the memory with the specified type and properties
uint32 Engine::graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
	VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
		if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
	}
	Exit("Failed to find suitable memory type");
}




luxDebug(VKAPI_ATTR VkBool32 VKAPI_CALL Engine::graphicsDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) Failure
	else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) Warning
		printf("Validation layer error\n %s", pCallbackData->pMessage);
	return VK_FALSE;
})
