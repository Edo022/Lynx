#pragma once
#define LUX_H_GSWAPCHAIN
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/CtArray.hpp"
// #include "LuxEngine/Core/Memory/Gpu/VMemory.hpp""
#include "LuxEngine/Core/Memory/Gpu/VMemory.hpp"







namespace lux::core::render::swapchain{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR	capabilities;
		RtArray<VkSurfaceFormatKHR>	formats;
		RtArray<VkPresentModeKHR>	presentModes;
	};


	extern VkSwapchainKHR			swapchain;				padExt(swapchain)
	extern RtArray<VkImage>			swapchainImages;		padExt(swapchainImages)
	extern RtArray<VkImageView>		swapchainImageViews;	padExt(swapchainImageViews)
	extern VkFormat					swapchainImageFormat;	padExt(swapchainImageFormat)
	extern VkExtent2D				swapchainExtent;		padExt(swapchainExtent)
	extern RtArray<VkFramebuffer>	swapchainFramebuffers;	padExt(swapchainFramebuffers)




	// void preInit( );
	void					swapchainCreate( );
	void					swapchainRecreate(const bool vWindowResized);
	void					cleanup( );
	VkSurfaceFormatKHR		swapchainChooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR		swapchainChoosePresentMode(const RtArray<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D				swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails	swapchainQuerySupport(const VkPhysicalDevice vDevice);
}
