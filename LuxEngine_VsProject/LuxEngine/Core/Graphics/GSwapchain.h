
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/LuxArray.h"





namespace lux::core::g{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		lux::Array<VkSurfaceFormatKHR>	formats;
		lux::Array<VkPresentModeKHR>	presentModes;
	};


	extern VkSwapchainKHR		swapchain;
	extern Array<VkImage>		swapchainImages;
	extern Array<VkImageView>	swapchainImageViews;
	extern VkFormat				swapchainImageFormat;
	extern VkExtent2D			swapchainExtent;
	extern Array<VkFramebuffer>	swapchainFramebuffers;


	void						swapchainCreate();
	void						swapchainRecreate(const bool vWindowResized);
	void						swapchainCleanup();
	VkSurfaceFormatKHR			swapchainChooseSurfaceFormat(const lux::Array<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR			swapchainChoosePresentMode(const lux::Array<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D					swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails		swapchainQuerySupport(const VkPhysicalDevice vDevice);
}
