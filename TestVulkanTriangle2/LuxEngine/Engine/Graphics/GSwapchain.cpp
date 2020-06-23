
#include "LuxEngine/Engine/Engine.h"




VkSurfaceFormatKHR Engine::chooseSwapSurfaceFormat(const LuxArray<VkSurfaceFormatKHR>* availableFormats) {
	for (const auto& availableFormat : *availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return (*availableFormats)[0];
}




//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
VkPresentModeKHR Engine::chooseSwapPresentMode(const LuxArray<VkPresentModeKHR>* availablePresentModes) {
	if (useVSync) return VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& availablePresentMode : *availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
	}
	return VK_PRESENT_MODE_IMMEDIATE_KHR;
}




VkExtent2D Engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return VkExtent2D{
		max(pCapabilities->minImageExtent.width, min(pCapabilities->maxImageExtent.width, sc<uint32>(width))),
		max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, sc<uint32>(height)))
	};
}





Engine::SwapChainSupportDetails Engine::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}




void Engine::createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());
	for (uint32 i = 0; i < swapChainImages.size(); ++i) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}








//-------------------------------------------------------------------------------------------------------------------------------------------------//








void Engine::createSwapChain() {
	//Get swapchain details
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(graphics.PD.device);Failure printf(">> %s at line %d", __func__, __LINE__);

	//Choose max image count. Minimum or minimum +1 if supported
	uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;Failure printf(">> %s at line %d", __func__, __LINE__);
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;Failure printf(">> %s at line %d", __func__, __LINE__);
	}


	//swapchain creation infos
	VkSwapchainCreateInfoKHR createInfo{};Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.surface = surface;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.minImageCount = imageCount;Failure printf(">> %s at line %d", __func__, __LINE__);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(&swapChainSupport.formats);Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.imageFormat = surfaceFormat.format;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.imageColorSpace = surfaceFormat.colorSpace;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.imageExtent = chooseSwapExtent(&swapChainSupport.capabilities);Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.imageArrayLayers = 1;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;Failure printf(">> %s at line %d", __func__, __LINE__);

	uint32 queueFamilyIndices[] = { graphics.PD.indices.graphicsFamily, graphics.PD.indices.presentFamily };
	if (graphics.PD.indices.graphicsFamily != graphics.PD.indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;Failure printf(">> %s at line %d", __func__, __LINE__);
		createInfo.queueFamilyIndexCount = 2;Failure printf(">> %s at line %d", __func__, __LINE__);
		createInfo.pQueueFamilyIndices = queueFamilyIndices;Failure printf(">> %s at line %d", __func__, __LINE__);
	}
	else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.presentMode = chooseSwapPresentMode(&swapChainSupport.presentModes);Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.clipped = VK_TRUE;Failure printf(">> %s at line %d", __func__, __LINE__);
	createInfo.oldSwapchain = VK_NULL_HANDLE;Failure printf(">> %s at line %d", __func__, __LINE__);


	//Create swapchain
	TryVk(vkCreateSwapchainKHR(graphics.LD, &createInfo, nullptr, &swapChain)) Exit("Failed to create swapchain");Failure printf(">> %s at line %d", __func__, __LINE__);


	//Save data
	uint32 swapchainImageCount;
	vkGetSwapchainImagesKHR(graphics.LD, swapChain, &swapchainImageCount, nullptr);					Failure printf(">> %s at line %d", __func__, __LINE__);//Get image count
	swapChainImages.resize(swapchainImageCount);Failure printf(">> %s at line %d", __func__, __LINE__);
	vkGetSwapchainImagesKHR(graphics.LD, swapChain, &swapchainImageCount, swapChainImages.data());	Failure printf(">> %s at line %d", __func__, __LINE__);//Save images
	swapChainImageFormat = surfaceFormat.format;													Failure printf(">> %s at line %d", __func__, __LINE__);//Save format
	swapChainExtent = createInfo.imageExtent;														Failure printf(">> %s at line %d", __func__, __LINE__);//Save extent


	createImageViews();Failure printf(">> %s at line %d", __func__, __LINE__);
	createRenderPass();Failure printf(">> %s at line %d", __func__, __LINE__);

	createFramebuffers();Failure printf(">> %s at line %d", __func__, __LINE__);
}








void Engine::cleanupSwapChain() {
	vkDestroyRenderPass(graphics.LD, renderPass, nullptr);			Failure printf(">> %s at line %d", __func__, __LINE__);//Destroy render pass

	for (auto framebuffer : swapChainFramebuffers) vkDestroyFramebuffer(graphics.LD, framebuffer, nullptr);				Failure printf(">> %s at line %d", __func__, __LINE__);//Destroy framebuffers
	vkFreeCommandBuffers(graphics.LD, graphicsCommandPool, sc<uint32>(commandBuffers.size()), commandBuffers.data());	Failure printf(">> %s at line %d", __func__, __LINE__);//Free graphics command buffers
	for (auto imageView : swapChainImageViews) vkDestroyImageView(graphics.LD, imageView, nullptr);						Failure printf(">> %s at line %d", __func__, __LINE__);//Destroy image views

	vkDestroySwapchainKHR(graphics.LD, swapChain, nullptr);																Failure printf(">> %s at line %d", __func__, __LINE__);//destroy swapchain
	destroyGpuCell(windowOutput); Failure printf(">> %s at line %d", __func__, __LINE__);
}






void Engine::recreateSwapChain() {
	windowResizeFence.wait(1); Failure printf(">> %s at line %d", __func__, __LINE__);//from framebufferResizeCallback
	int32 width, height;
	glfwGetFramebufferSize(window, &width, &height); Failure printf(">> %s at line %d", __func__, __LINE__);

	if (width != 0 && height != 0) {
		vkDeviceWaitIdle(graphics.LD); Failure printf(">> %s at line %d", __func__, __LINE__);
		cleanupSwapChain(); Failure printf(">> %s at line %d", __func__, __LINE__);
		createSwapChain(); Failure printf(">> %s at line %d", __func__, __LINE__);

		uint32* pwindowSize = (uint32*)mapGpuBuffer(windowSize); pwindowSize[0] = swapChainExtent.width, pwindowSize[1] = swapChainExtent.height; Failure printf(">> %s at line %d", __func__, __LINE__);
		windowOutput = createGpuCell(swapChainExtent.width * swapChainExtent.height * 4/*A8-R8-G8-B8*/, false); Failure printf(">> %s at line %d", __func__, __LINE__);
		//free(pwindowSize);
		//vkUnmapMemory(compute.LD, CBuffers[__lp_buffer_from_cc(windowSize)].memory);


		CShader_destroy(0); Failure printf(">> %s at line %d", __func__, __LINE__);
		LuxArray<LuxCell> cells = { windowOutput, test___, windowSize }; Failure printf(">> %s at line %d", __func__, __LINE__);
		CShader_new(&cells, "LuxEngine/Contents/shaders/comp.spv"); Failure printf(">> %s at line %d", __func__, __LINE__);
	}
	windowResizeFence.set(2); Failure printf(">> %s at line %d", __func__, __LINE__);
}
