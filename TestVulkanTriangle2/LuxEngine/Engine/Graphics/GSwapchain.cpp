
#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader.struct.h"       



VkSurfaceFormatKHR Engine::swapchainChooseSurfaceFormat(const LuxArray<VkSurfaceFormatKHR>* pAvailableFormats) {
	for (const auto& availableFormat : *pAvailableFormats) {
		if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return (*pAvailableFormats)[0];
}




//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
VkPresentModeKHR Engine::swapchainChoosePresentMode(const LuxArray<VkPresentModeKHR>* pAvailablePresentModes) {
	if (useVSync) return VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& availablePresentMode : *pAvailablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
	}
	return VK_PRESENT_MODE_IMMEDIATE_KHR;
}




VkExtent2D Engine::swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return VkExtent2D{
		max(pCapabilities->minImageExtent.width, min(pCapabilities->maxImageExtent.width, scast<uint32>(width))),
		max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, scast<uint32>(height)))
	};
}





Engine::SwapChainSupportDetails Engine::swapchainQuerySupport(const VkPhysicalDevice vDevice) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vDevice, surface, &details.capabilities);

	uint32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vDevice, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vDevice, surface, &formatCount, details.formats.data());
	}

	uint32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vDevice, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}








//-------------------------------------------------------------------------------------------------------------------------------------------------//








void Engine::swapchainCreate() {
	//Get swapchain details
	SwapChainSupportDetails swapChainSupport = swapchainQuerySupport(graphics.PD.device);

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
	VkSurfaceFormatKHR surfaceFormat = swapchainChooseSurfaceFormat(&swapChainSupport.formats);
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchainChooseExtent(&swapChainSupport.capabilities);
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
	createInfo.presentMode = swapchainChoosePresentMode(&swapChainSupport.presentModes);
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;


	//Create swapchain
	TryVk(vkCreateSwapchainKHR(graphics.LD, &createInfo, nullptr, &swapchain)) Exit("Failed to create swapchain");


	//Save data
	uint32 swapchainImageCount;
	vkGetSwapchainImagesKHR(graphics.LD, swapchain, &swapchainImageCount, nullptr);					//Get image count
	swapchainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(graphics.LD, swapchain, &swapchainImageCount, swapchainImages.data());	//Save images
	swapchainImageFormat = surfaceFormat.format;													//Save format
	swapchainExtent = createInfo.imageExtent;														//Save extent


	//Create image views
	swapchainImageViews.resize(swapchainImages.size());
	for (uint32 i = 0; i < swapchainImages.size(); ++i) swapchainImageViews[i] = swapchainCreateImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


	createRenderPass();
	createFramebuffers();
}








void Engine::swapchainCleanup() {
	vkDestroyRenderPass(graphics.LD, renderPass, nullptr);														//Destroy render pass
	for (auto framebuffer : swapchainFramebuffers) vkDestroyFramebuffer(graphics.LD, framebuffer, nullptr);	//Destroy framebuffers
	for (auto imageView : swapchainImageViews) vkDestroyImageView(graphics.LD, imageView, nullptr);			//Destroy image views
	vkDestroySwapchainKHR(graphics.LD, swapchain, nullptr);													//destroy swapchain
}






void Engine::swapchainRecreate(const bool vWindowResized) {
	if (vWindowResized) windowResizeFence.wait(1); //from framebufferResizeCallback
	int32 width, height;
	glfwGetFramebufferSize(window, &width, &height);

	if (width != 0 && height != 0) {
		vkDeviceWaitIdle(graphics.LD);
		swapchainCleanup();
		swapchainCreate();


		uint32* pwindowSize = rcast<uint32*>(gpuCellMap(__windowSize));
		pwindowSize[0] = swapchainExtent.width;
		pwindowSize[1] = swapchainExtent.height;

		//TODO dont resize always
		gpuCellDestroy(__windowOutput);
		__windowOutput = gpuCellCreate(swapchainExtent.width * swapchainExtent.height * 4/*A8-R8-G8-B8*/, false);

		//TODO dont resize always
		cshaderDestroy(testShader0);
		LuxArray<LuxCell> cells = { __windowOutput, __windowSize, __vertices };
		testShader0 = cshaderNew(&cells, "LuxEngine/Contents/shaders/comp.spv");


		{ //destroy copy command buffers
			//Clear command buffer, command pool and useless pointers
			vkFreeCommandBuffers(compute.LD, CShaders[copyShader].commandPool, CShaders[copyShader].commandBuffers.__lp_size, CShaders[copyShader].commandBuffers.data());
			vkDestroyCommandPool(compute.LD, CShaders[copyShader].commandPool, null);
			forEach(CShaders[copyShader].__lp_ptrs, i) free(CShaders[copyShader].__lp_ptrs[i]);
			CShaders.remove(copyShader);										//Remove the shader from the shader array
		}
		{ //Create copy command buffers #LLID CCB0000
			copyShader = CShaders.add(LuxCShader{});							//Add the shader to the shader array
			CShaders[copyShader].commandBuffers.resize(swapchainImages.size());//Resize the command buffer array in the shader
			__lp_cshaderCreateCopyCommandBuffers();									//Create command buffers and command pool
		}
	}
	if (vWindowResized) windowResizeFence.set(2);
}
