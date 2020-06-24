
#include "LuxEngine/Engine/Engine.h"




VkSurfaceFormatKHR Engine::chooseSwapSurfaceFormat(const LuxArray<VkSurfaceFormatKHR>* availableFormats) {
	for (const auto& availableFormat : *availableFormats) {
		if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(graphics.PD.device);

	//Choose max image count. Minimum or minimum +1 if supported
	uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}


	//swapchain creation infos
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(&swapChainSupport.formats);
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = chooseSwapExtent(&swapChainSupport.capabilities);
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	uint32 queueFamilyIndices[] = { graphics.PD.indices.graphicsFamily, graphics.PD.indices.presentFamily };
	if (graphics.PD.indices.graphicsFamily != graphics.PD.indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = chooseSwapPresentMode(&swapChainSupport.presentModes);
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;


	//Create swapchain
	TryVk(vkCreateSwapchainKHR(graphics.LD, &createInfo, nullptr, &swapChain)) Exit("Failed to create swapchain");


	//Save data
	uint32 swapchainImageCount;
	vkGetSwapchainImagesKHR(graphics.LD, swapChain, &swapchainImageCount, nullptr);					//Get image count
	swapChainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(graphics.LD, swapChain, &swapchainImageCount, swapChainImages.data());	//Save images
	swapChainImageFormat = surfaceFormat.format;													//Save format
	swapChainExtent = createInfo.imageExtent;														//Save extent


	createImageViews();
	createRenderPass();

	createFramebuffers();
}








void Engine::cleanupSwapChain() {
	vkDestroyRenderPass(graphics.LD, renderPass, nullptr);			//Destroy render pass

	for (auto framebuffer : swapChainFramebuffers) vkDestroyFramebuffer(graphics.LD, framebuffer, nullptr);				//Destroy framebuffers
	vkFreeCommandBuffers(graphics.LD, graphicsCommandPool, sc<uint32>(commandBuffers.size()), commandBuffers.data());	//Free graphics command buffers
	for (auto imageView : swapChainImageViews) vkDestroyImageView(graphics.LD, imageView, nullptr);						//Destroy image views

	vkDestroySwapchainKHR(graphics.LD, swapChain, nullptr);																//destroy swapchain
	destroyGpuCell(__windowOutput); 
}






void Engine::recreateSwapChain(bool windowResized) {
	//TODO wait only if resized. when the process goes here the fence can be 2 and it dies
	if(windowResized) windowResizeFence.wait(1); //from framebufferResizeCallback
		int32 width, height;
		glfwGetFramebufferSize(window, &width, &height); 

		if (width != 0 && height != 0) {
			vkDeviceWaitIdle(graphics.LD); 
				cleanupSwapChain(); 
				createSwapChain(); 

				uint32* pwindowSize = (uint32*)mapGpuBuffer(__windowSize); pwindowSize[0] = swapChainExtent.width, pwindowSize[1] = swapChainExtent.height; 
				__windowOutput = createGpuCell(swapChainExtent.width * swapChainExtent.height * 4/*A8-R8-G8-B8*/, false); 
				//free(pwindowSize);
				//vkUnmapMemory(compute.LD, CBuffers[__lp_buffer_from_cc(windowSize)].memory);


				CShader_destroy(0); 
			LuxArray<LuxCell> cells = { __windowOutput, __vertices, __windowSize }; 
			CShader_new(&cells, "LuxEngine/Contents/shaders/comp.spv"); 
		}
	if (windowResized) windowResizeFence.set(2); 
}
