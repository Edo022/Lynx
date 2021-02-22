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


	extern VkSwapchainKHR			swapchain;
	extern RtArray<VkImage>			swapchainImages;
	extern RtArray<VkImageView>		swapchainImageViews;
	extern VkFormat					swapchainImageFormat;
	extern VkExtent2D				swapchainExtent;
	extern RtArray<VkFramebuffer>	swapchainFramebuffers;




	// void preInit( );
	void					swapchainCreate( );
	void					swapchainRecreate(const bool vWindowResized);
	void					cleanup( );
	VkSurfaceFormatKHR		swapchainChooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR		swapchainChoosePresentMode(const RtArray<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D				swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails	swapchainQuerySupport(const VkPhysicalDevice vDevice);
}
