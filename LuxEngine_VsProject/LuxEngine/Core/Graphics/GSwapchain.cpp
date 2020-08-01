

#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/CShader_t.h"


VkSurfaceFormatKHR Engine::swapchainChooseSurfaceFormat(const lux::Array<VkSurfaceFormatKHR>* pAvailableFormats) {
	for(const auto& availableFormat : *pAvailableFormats) {
		//if(availableFormat.format == VK_FORMAT_R32G32B32A32_SFLOAT && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		if(availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return (*pAvailableFormats)[0];
}




//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
VkPresentModeKHR Engine::swapchainChoosePresentMode(const lux::Array<VkPresentModeKHR>* pAvailablePresentModes) {
	if(useVSync) return VK_PRESENT_MODE_FIFO_KHR;
	for(const auto& availablePresentMode : *pAvailablePresentModes) {
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
	}
	return VK_PRESENT_MODE_IMMEDIATE_KHR;
}




VkExtent2D Engine::swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
	int32 width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return VkExtent2D{
		max(pCapabilities->minImageExtent.width, min(pCapabilities->maxImageExtent.width, (uint32)width)),
		max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, (uint32)height))
	};
}





Engine::SwapChainSupportDetails Engine::swapchainQuerySupport(const VkPhysicalDevice vDevice) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vDevice, surface, &details.capabilities);

	uint32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vDevice, surface, &formatCount, nullptr);
	if(formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vDevice, surface, &formatCount, details.formats.begin( ));
	}

	uint32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vDevice, surface, &presentModeCount, nullptr);
	if(presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vDevice, surface, &presentModeCount, details.presentModes.begin( ));
	}

	return details;
}








//-------------------------------------------------------------------------------------------------------------------------------------------------//








void Engine::swapchainCreate( ) {
	//Get swapchain details
	SwapChainSupportDetails swapChainSupport = swapchainQuerySupport(graphics.PD.device);

	//Choose max image count. Minimum or minimum +1 if supported
	uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}


	//swapchain creation infos
	VkSurfaceFormatKHR surfaceFormat{ swapchainChooseSurfaceFormat(&swapChainSupport.formats) };
	VkSwapchainCreateInfoKHR createInfo{
		.sType{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
		.surface{ surface },
		.minImageCount{ imageCount },
		.imageFormat{ surfaceFormat.format },
		.imageColorSpace{ surfaceFormat.colorSpace },
		.imageExtent{ swapchainChooseExtent(&swapChainSupport.capabilities) },
		.imageArrayLayers{ 1 },
		.imageUsage{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT },
	};

	uint32 queueFamilyIndices[] = { graphics.PD.indices.graphicsFamily, graphics.PD.indices.presentFamily };
	if(graphics.PD.indices.graphicsFamily != graphics.PD.indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = swapchainChoosePresentMode(&swapChainSupport.presentModes);
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;


	//Create swapchain
	TryVk(vkCreateSwapchainKHR(graphics.LD, &createInfo, nullptr, &swapchain)) Exit("Failed to create swapchain");


	//Save data
	uint32 swapchainImageCount;
	vkGetSwapchainImagesKHR(graphics.LD, swapchain, &swapchainImageCount, nullptr);					//Get image count
	swapchainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(graphics.LD, swapchain, &swapchainImageCount, swapchainImages.begin( ));	//Save images
	swapchainImageFormat = surfaceFormat.format;													//Save format
	swapchainExtent = createInfo.imageExtent;														//Save extent


	//Create image views
	swapchainImageViews.resize(swapchainImages.size( ));
	for(uint32 i = 0; i < swapchainImages.size( ); ++i) swapchainImageViews[i] = swapchainCreateImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


	createRenderPass( );
	createFramebuffers( );
}








void Engine::swapchainCleanup( ) {
	vkDestroyRenderPass(graphics.LD, renderPass, nullptr);													//Destroy render pass
	for(auto framebuffer : swapchainFramebuffers) vkDestroyFramebuffer(graphics.LD, framebuffer, nullptr);	//Destroy framebuffers
	for(auto imageView : swapchainImageViews) vkDestroyImageView(graphics.LD, imageView, nullptr);			//Destroy image views
	vkDestroySwapchainKHR(graphics.LD, swapchain, nullptr);													//destroy swapchain
}






void Engine::swapchainRecreate(const bool vWindowResized) {
	if(vWindowResized) windowResizeFence.startFirst( );
	int32 width, height;
	glfwGetFramebufferSize(window, &width, &height);

	if(width != 0 && height != 0) {
		vkDeviceWaitIdle(graphics.LD);
		swapchainCleanup( );
		swapchainCreate( );


		{ //destroy copy command buffers
			vkFreeCommandBuffers(compute.LD, copyCommandPool, copyCommandBuffers.size( ), copyCommandBuffers.begin( ));
			vkDestroyCommandPool(compute.LD, copyCommandPool, nullptr);
		}

		uint32* pwindowSize = scast<uint32*>(gpuCellMap(gpuCellWindowSize));
		pwindowSize[0] = swapchainExtent.width;
		pwindowSize[1] = swapchainExtent.height;

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(swapchainImages.size( ));	//Resize the command buffer array in the shader
			cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
		}
	}
	if(vWindowResized) windowResizeFence.endFirst( );
}
