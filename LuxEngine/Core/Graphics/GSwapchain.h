
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Memory/Gpu/VMemory.h"
#define LUX_H_GSWAPCHAIN






namespace lux::core::g::swapchain{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR	capabilities;
		DynArray<VkSurfaceFormatKHR>	formats;
		DynArray<VkPresentModeKHR>		presentModes;
	};


	extern VkSwapchainKHR		swapchain;
	extern DynArray<VkImage>		swapchainImages;
	extern DynArray<VkImageView>	swapchainImageViews;
	extern VkFormat				swapchainImageFormat;
	extern VkExtent2D			swapchainExtent;
	extern DynArray<VkFramebuffer>	swapchainFramebuffers;




	void preInit( );
	void						swapchainCreate( );
	void						swapchainRecreate(const bool vWindowResized);
	void						cleanup( );
	VkSurfaceFormatKHR			swapchainChooseSurfaceFormat(const DynArray<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR			swapchainChoosePresentMode(const DynArray<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D					swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails		swapchainQuerySupport(const VkPhysicalDevice vDevice);
}
