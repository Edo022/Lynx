
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Core/Core.h"








namespace lux::core::g::swapchain{
	VkSwapchainKHR			swapchain;
	Array<VkImage>			swapchainImages;
	Array<VkImageView>		swapchainImageViews;
	VkFormat				swapchainImageFormat;
	VkExtent2D				swapchainExtent;
	Array<VkFramebuffer>	swapchainFramebuffers;








	VkSurfaceFormatKHR swapchainChooseSurfaceFormat(const Array<VkSurfaceFormatKHR>* pAvailableFormats) {
		for(const auto& availableFormat : *pAvailableFormats) {
			//TODO use best format available when not specified
			//TODO use RGBA8 format in shaders when better formats are not available
			if(availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return (*pAvailableFormats)[0];
	}




	//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
	VkPresentModeKHR swapchainChoosePresentMode(const Array<VkPresentModeKHR>* pAvailablePresentModes) {
		if(useVSync) return VK_PRESENT_MODE_FIFO_KHR;
		for(const auto& availablePresentMode : *pAvailablePresentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
		}
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}




	VkExtent2D swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
		int32 width, height;
		glfwGetFramebufferSize(wnd::window, &width, &height);
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
		SwapChainSupportDetails swapChainSupport = swapchainQuerySupport(dvc::graphics.PD.device);

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

		uint32 queueFamilyIndices[] = { dvc::graphics.PD.indices.graphicsFamily, dvc::graphics.PD.indices.presentFamily };
		if(dvc::graphics.PD.indices.graphicsFamily != dvc::graphics.PD.indices.presentFamily) {
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
		TryVk(vkCreateSwapchainKHR(dvc::graphics.LD, &createInfo, nullptr, &swapchain)) printError("Failed to create swapchain", true, -1);


		//Save data
		uint32 swapchainImageCount;
		vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &swapchainImageCount, nullptr);					//Get image count
		swapchainImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &swapchainImageCount, swapchainImages.begin( ));	//Save images
		swapchainImageFormat = surfaceFormat.format;															//Save format
		swapchainExtent = createInfo.imageExtent;																//Save extent


		//Create image views
		swapchainImageViews.resize(swapchainImages.size( ));
		for(uint32 i = 0; i < swapchainImages.size( ); ++i) swapchainImageViews[i] = out::swapchainCreateImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


		out::createRenderPass( );
		out::createFramebuffers( );
	}








	void cleanup( ) {
		vkDestroyRenderPass(dvc::graphics.LD, out::renderPass, nullptr);												//Destroy render pass
		for(auto framebuffer : swapchainFramebuffers) vkDestroyFramebuffer(dvc::graphics.LD, framebuffer, nullptr);		//Destroy framebuffers
		for(auto imageView : swapchainImageViews) vkDestroyImageView(dvc::graphics.LD, imageView, nullptr);				//Destroy image views
		vkDestroySwapchainKHR(dvc::graphics.LD, swapchain, nullptr);													//destroy swapchain
	}








	void swapchainRecreate(const bool vWindowResized) {
		if(vWindowResized) wnd::windowResizeFence.startFirst( );	//Sync with framebufferResizeCallback

		//TODO dont destroy it every time
		static int32 width, height;	glfwGetFramebufferSize(wnd::window, &width, &height);
		if(width != 0 && height != 0) {			//If the window contains pixels
			vkDeviceWaitIdle(dvc::graphics.LD);		//Wait for the logical device
			swapchain::cleanup( );					//Clean the old swapchain
			swapchain::swapchainCreate( );			//Create a new swapchain


			//Update the window size buffer
			uint32* pwindowSize = scast<uint32*>(c::buffers::gpuCellMap(wnd::gpuCellWindowSize));
			pwindowSize[0] = swapchainExtent.width;
			pwindowSize[1] = swapchainExtent.height;

			{	//Destroy copy command buffers
				vkFreeCommandBuffers(dvc::compute.LD, c::copyCommandPool, c::copyCommandBuffers.size( ), c::copyCommandBuffers.begin( ));
				vkDestroyCommandPool(dvc::compute.LD, c::copyCommandPool, nullptr);

				//#LLID CCB0000 Recreate copy command buffers
				c::copyCommandBuffers.resize(swapchainImages.size( ));	//Resize the command buffer array in the shader
				c::shaders::createDefaultCommandBuffers( );				//Create command buffers and command pool
			}

			//Recreate clear shader
			c::shaders::updateShaderCall(c::shaders::clearShader, LUX_DEF_SHADER_CLEAR, (swapchainExtent.width * swapchainExtent.height) / (32 * 32) + 1, 1, 1);
		}
		if(vWindowResized) wnd::windowResizeFence.endFirst( );		//Sync with framebufferResizeCallback
	}
}