#pragma once
#define LUX_H_SWAPCHAIN
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/CtArray.hpp"







namespace lux::core::wnd{
	struct Swapchain{
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR	capabilities;
			RtArray<VkSurfaceFormatKHR>	formats;
			RtArray<VkPresentModeKHR>	presentModes;
		};


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
		static SwapChainSupportDetails	swapchainQuerySupport(const VkPhysicalDevice vDevice);

		~Swapchain(){
			vkDestroySwapchainKHR(dvc::graphics.LD, swapchain, nullptr);
		}
	};
}
