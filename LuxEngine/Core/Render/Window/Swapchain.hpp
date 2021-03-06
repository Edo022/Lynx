#pragma once
#define LUX_H_SWAPCHAIN
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/CtArray.hpp"







namespace lux::core::wnd{
	struct Swapchain{
		VkSwapchainKHR			swapchain = nullptr;
		RtArray<VkImage>		swapchainImages;
		RtArray<VkImageView>	swapchainImageViews;
		VkFormat				swapchainImageFormat;
		VkExtent2D				swapchainExtent = {};
		RtArray<VkFramebuffer>	swapchainFramebuffers;




		// void preInit();
		void					swapchainCreate();
		void					swapchainRecreate(const bool vWindowResized);
		void					cleanup();
		VkSurfaceFormatKHR		swapchainChooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>* pAvailableFormats);
		VkPresentModeKHR		swapchainChoosePresentMode(const RtArray<VkPresentModeKHR>* pAvailablePresentModes);
		VkExtent2D				swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);

		~Swapchain(){
			vkDestroySwapchainKHR(dvc::graphics.LD, swapchain, nullptr);
		}
	};








	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR	capabilities;
		RtArray<VkSurfaceFormatKHR>	formats;
		RtArray<VkPresentModeKHR>	presentModes;
	};
	static SwapChainSupportDetails getSwapchainSupportDetails(const VkPhysicalDevice vDevice, const VkSurfaceKHR vSurface) {
		SwapChainSupportDetails details;

		//Get surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vDevice, vSurface, &details.capabilities);

		//Get surface formats
		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(vDevice, vSurface, &formatCount, nullptr);
		if(formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(vDevice, vSurface, &formatCount, details.formats.begin());
		}

		//Get surface present modes
		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vDevice, vSurface, &presentModeCount, nullptr);
		if(presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(vDevice, vSurface, &presentModeCount, details.presentModes.begin());
		}

		return details;
	}
}
