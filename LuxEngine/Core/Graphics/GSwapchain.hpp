#pragma once
#define LUX_H_GSWAPCHAIN
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/LuxArray.hpp"
#include "LuxEngine/Memory/Gpu/VMemory.hpp"






namespace lux::core::g::swapchain{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR	capabilities;
		RTArray<VkSurfaceFormatKHR>	formats;
		RTArray<VkPresentModeKHR>	presentModes;
	};


	extern VkSwapchainKHR			swapchain;
	extern RTArray<VkImage>			swapchainImages;
	extern RTArray<VkImageView>		swapchainImageViews;
	extern VkFormat					swapchainImageFormat;
	extern VkExtent2D				swapchainExtent;
	extern RTArray<VkFramebuffer>	swapchainFramebuffers;




	void preInit( );
	void					swapchainCreate( );
	void					swapchainRecreate(const bool vWindowResized);
	void					cleanup( );
	VkSurfaceFormatKHR		swapchainChooseSurfaceFormat(const RTArray<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR		swapchainChoosePresentMode(const RTArray<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D				swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails	swapchainQuerySupport(const VkPhysicalDevice vDevice);
}
