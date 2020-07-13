
#include <vulkan/vulkan.h>
#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"


void Engine::runGraphics(const bool vUseVSync, const float vFOV) {
	useVSync = vUseVSync;
	FOV = vFOV;

	luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;
	Normal  printf("Initializing Vulkan");							graphicsInitVulkan();						MainSeparator;
}




void Engine::graphicsInitVulkan() {
	//Initialize vulkan
	Normal printf("    Creating VK Surface...               ");		graphicsCreateSurface();					SuccessNoNl printf("ok");	NewLine;
	Normal printf("    Searching for physical devices...    ");		deviceGetPhysical();											NewLine;
	Normal printf("    Creating VK command pool...          ");		createGraphicsCommandPool();		SuccessNoNl printf("ok");
	Normal printf("    Creating VK swapchain...             ");		swapchainCreate();					SuccessNoNl printf("ok");

	luxDebug(graphicsCreateDebugMessenger());
	graphicsCreateFences();
}




inline void Engine::graphicsCreateSurface() {
	TryVk(glfwCreateWindowSurface(instance, window, nullptr, &surface)) Exit("Failed to create window surface");
}


void Engine::graphicsCreateDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	lux::_engine::populateDebugMessengerCreateInfo(createInfo);
	TryVk(lux::_engine::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Exit("Failed to set up debug messenger");
}





void Engine::graphicsCreateFences() {
	renderSemaphoreImageAvailable.resize(renderMaxFramesInFlight);
	renderSemaphoreFinished.resize(renderMaxFramesInFlight);
	renderFencesInFlight.resize(renderMaxFramesInFlight);
	renderFencesImagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int32 i = 0; i < renderMaxFramesInFlight; ++i) {
		if (vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &renderSemaphoreImageAvailable[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &renderSemaphoreFinished[i]) != VK_SUCCESS ||
			vkCreateFence(graphics.LD, &fenceInfo, nullptr, &renderFencesInFlight[i]) != VK_SUCCESS) {
			Exit("Failed to create synchronization objects for a frame");
		}
	}
}






//TODO multithreaded submit and command creation
void Engine::graphicsDrawFrame() {
	redraw:

	//Wait fences
	vkWaitForFences(graphics.LD, 1, &renderFencesInFlight[renderCurrentFrame], VK_TRUE, UINT64_MAX);
	if (renderFramebufferResized) {
		renderFramebufferResized = false;
		swapchainRecreate(true);
		goto redraw;
	}


	uint32 imageIndex;
	{ //Acquire swapchain image
		switch (vkAcquireNextImageKHR(graphics.LD, swapchain, UINT64_MAX, renderSemaphoreImageAvailable[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
			case VK_SUBOPTIMAL_KHR: case VK_SUCCESS:  break;
			case VK_ERROR_OUT_OF_DATE_KHR: swapchainRecreate(false);  return;
			default:  Exit("Failed to acquire swapchain image");
		}
		if (renderFencesImagesInFlight[imageIndex] != VK_NULL_HANDLE) vkWaitForFences(graphics.LD, 1, &renderFencesImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		renderFencesImagesInFlight[imageIndex] = renderFencesInFlight[renderCurrentFrame];
	}


	//TODO don't recreate the command buffer array every time 
	{ //Update render result submitting the command buffers to the compute queues
		while(CShaders_stg.size( ) > 0) {
			CShaders.add(CShaders_stg.front()->shader);
			delete(CShaders_stg.front());
			CShaders_stg.popFront();
		}

		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		lux::Array<VkCommandBuffer> commandBuffers(CShaders.usedSize() + 2);

		for (uint32 i = 0; i < CShaders.size(); ++i) {
			//TODO EXCEPTION 
			if (CShaders.isValid(i)) {
				commandBuffers[i + 1] = CShaders[i].commandBuffers.__lp_data[0];
			}
		}
		commandBuffers[commandBuffers.size() - 1] = copyCommandBuffers[imageIndex];
		commandBuffers[0] = clearCommandBuffer;

		static VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &renderSemaphoreImageAvailable[renderCurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderSemaphoreFinished[renderCurrentFrame];
		submitInfo.commandBufferCount = commandBuffers.size();
		submitInfo.pCommandBuffers = commandBuffers.begin();
		submitInfo.pWaitDstStageMask = waitStages;

		vkResetFences(graphics.LD, 1, &renderFencesInFlight[renderCurrentFrame]);
		TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, renderFencesInFlight[renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
	}


	{ //Present
		static VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderSemaphoreFinished[renderCurrentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		switch (vkQueuePresentKHR(graphics.presentQueue, &presentInfo)) {
			case VK_SUCCESS:  break;
			case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
				swapchainRecreate(false);
				vkDeviceWaitIdle(graphics.LD);
				break;
			}
			default:  Exit("Failed to present swapchain image");
		}

		//Update frame number
		renderCurrentFrame = (renderCurrentFrame + 1) % (renderMaxFramesInFlight);
		glfwSwapBuffers(window); 
	}
}




void Engine::framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
	engine.windowResizeFence.wait(0);  //from the last call of this function

	engine.renderFramebufferResized = true;
	engine.windowResizeFence.set(1);

	engine.windowResizeFence.wait(2);  //from RecreateSwapchain()
	engine.windowResizeFence.set(0);
}




void Engine::graphicsCleanup() {
	swapchainCleanup();																//Clear swapchain components
	vkDestroyCommandPool(graphics.LD, singleTimeCommandPool, nullptr);				//Destroy graphics command pool

	for (int32 i = 0; i < renderMaxFramesInFlight; ++i) {								//For every frame
		vkDestroySemaphore(graphics.LD, renderSemaphoreFinished[i], nullptr);			//Destroy his render semaphore
		vkDestroySemaphore(graphics.LD, renderSemaphoreImageAvailable[i], nullptr);			//Destroy his image  semaphore
		vkDestroyFence(graphics.LD, renderFencesInFlight[i], nullptr);						//Destroy his fence
	}


	if (graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
	vkDestroyDevice(compute.LD, nullptr);																			//Destroy the compute device
	//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);											//Destroy all the secondary devices

	luxDebug(lux::_engine::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));						//Destroy the debug messenger if present
	vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
}








// Other ------------------------------------------------------------------------------------------------------------------------------------//








VkFormat Engine::graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
	for (VkFormat format : *pCandidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

		if ((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
			(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
			return format;
		}
	}
	Exit("Failed to find a supported format");
}




//Returns the index of the memory with the specified type and properties. Exits if not found
uint32 Engine::graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
	VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {			//Search for the memory that has the specified properties and type and return its index
		if ((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
	}
	Exit("Failed to find suitable memory type");
}




VKAPI_ATTR VkBool32 VKAPI_CALL Engine::graphicsDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
