
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Core/Core.h"








namespace lux::core::g{
	VkSwapchainKHR			swapchain;
	Array<VkImage>			swapchainImages;
	Array<VkImageView>		swapchainImageViews;
	VkFormat				swapchainImageFormat;
	VkExtent2D				swapchainExtent;
	Array<VkFramebuffer>	swapchainFramebuffers;








	VkSurfaceFormatKHR swapchainChooseSurfaceFormat(const lux::Array<VkSurfaceFormatKHR>* pAvailableFormats) {
		for(const auto& availableFormat : *pAvailableFormats) {
			//if(availableFormat.format == VK_FORMAT_R32G32B32A32_SFLOAT && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			if(availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return (*pAvailableFormats)[0];
	}




	//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
	VkPresentModeKHR swapchainChoosePresentMode(const lux::Array<VkPresentModeKHR>* pAvailablePresentModes) {
		if(useVSync) return VK_PRESENT_MODE_FIFO_KHR;
		for(const auto& availablePresentMode : *pAvailablePresentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
		}
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}




	VkExtent2D swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
		int32 width, height;
		glfwGetFramebufferSize(g::window, &width, &height);
		return VkExtent2D{
			max(pCapabilities->minImageExtent.width, min(pCapabilities->maxImageExtent.width, (uint32)width)),
			max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, (uint32)height))
		};
	}





	SwapChainSupportDetails swapchainQuerySupport(const VkPhysicalDevice vDevice) {
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








	void swapchainCreate( ) {
		//Get swapchain details
		SwapChainSupportDetails swapChainSupport = swapchainQuerySupport(g::graphics.PD.device);

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

		uint32 queueFamilyIndices[] = { g::graphics.PD.indices.graphicsFamily, g::graphics.PD.indices.presentFamily };
		if(g::graphics.PD.indices.graphicsFamily != g::graphics.PD.indices.presentFamily) {
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
		TryVk(vkCreateSwapchainKHR(g::graphics.LD, &createInfo, nullptr, &g::swapchain)) Exit("Failed to create swapchain");


		//Save data
		uint32 swapchainImageCount;
		vkGetSwapchainImagesKHR(g::graphics.LD, g::swapchain, &swapchainImageCount, nullptr);					//Get image count
		g::swapchainImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(g::graphics.LD, g::swapchain, &swapchainImageCount, g::swapchainImages.begin( ));	//Save images
		g::swapchainImageFormat = surfaceFormat.format;													//Save format
		g::swapchainExtent = createInfo.imageExtent;														//Save extent


		//Create image views
		g::swapchainImageViews.resize(g::swapchainImages.size( ));
		for(uint32 i = 0; i < g::swapchainImages.size( ); ++i) g::swapchainImageViews[i] = g::swapchainCreateImageView(g::swapchainImages[i], g::swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


		g::createRenderPass( );
		g::createFramebuffers( );
	}








	void swapchainCleanup( ) {
		vkDestroyRenderPass(g::graphics.LD, g::renderPass, nullptr);													//Destroy render pass
		for(auto framebuffer : g::swapchainFramebuffers) vkDestroyFramebuffer(g::graphics.LD, framebuffer, nullptr);	//Destroy framebuffers
		for(auto imageView : g::swapchainImageViews) vkDestroyImageView(g::graphics.LD, imageView, nullptr);			//Destroy image viewslux::core::g::compute
		vkDestroySwapchainKHR(g::graphics.LD, g::swapchain, nullptr);													//destroy swapchain
	}






	void swapchainRecreate(const bool vWindowResized) {
		if(vWindowResized) g::windowResizeFence.startFirst( );
		int32 width, height;
		glfwGetFramebufferSize(g::window, &width, &height);

		if(width != 0 && height != 0) {
			vkDeviceWaitIdle(g::graphics.LD);
			swapchainCleanup( );
			swapchainCreate( );


			{ //destroy copy command buffers
				vkFreeCommandBuffers(g::compute.LD, c::copyCommandPool, c::copyCommandBuffers.size( ), c::copyCommandBuffers.begin( ));
				vkDestroyCommandPool(g::compute.LD, c::copyCommandPool, nullptr);
			}

			uint32* pwindowSize = scast<uint32*>(c::gpuCellMap(g::gpuCellWindowSize));
			pwindowSize[0] = g::swapchainExtent.width;
			pwindowSize[1] = g::swapchainExtent.height;

			{ //#LLID CCB0000 Create copy command buffers
				c::copyCommandBuffers.resize(g::swapchainImages.size( ));	//Resize the command buffer array in the shader
				c::cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
			}
		}
		if(vWindowResized) g::windowResizeFence.endFirst( );
	}
}