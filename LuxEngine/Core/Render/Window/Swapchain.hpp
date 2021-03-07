#pragma once
#define LUX_H_SWAPCHAIN
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/CtArray.hpp"






namespace lux{
	struct Window;
}
namespace lux::core::wnd{
		constexpr int32			__renderMaxFramesInFlight = 2;	//Default:2


	struct Swapchain{
		VkSwapchainKHR			swapchain = nullptr;
		RtArray<VkImage>		swapchainImages;
		RtArray<VkImageView>	swapchainImageViews;
		VkFormat				swapchainImageFormat;
		VkExtent2D				swapchainExtent = {};
		RtArray<VkFramebuffer>	swapchainFramebuffers;

		VkRenderPass	renderPass               = nullptr;
	bool			renderFramebufferResized = false;


		Window* bindedWindow;


	RtArray<VkSemaphore> s_imageAquired   ;//(__renderMaxFramesInFlight);
	RtArray<VkSemaphore> s_objectsRendered;//(__renderMaxFramesInFlight);
	RtArray<VkSemaphore> s_copy           ;//(__renderMaxFramesInFlight);
	RtArray<VkSemaphore> s_clear          ;//(__renderMaxFramesInFlight);
	RtArray<VkFence>     imageRendered_f  ;//(__renderMaxFramesInFlight);
	int32                renderCurrentFrame = 0;

		// Swapchain(){ swapchainCreate(); }



		// void preInit();
		void					swapchainCreate();
				void ___init(const bool vUseVSync);
		void					swapchainRecreate(const bool vWindowResized);
		void					cleanup();
		VkSurfaceFormatKHR		swapchainChooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>& pAvailableFormats);
		VkPresentModeKHR		swapchainChoosePresentMode(const RtArray<VkPresentModeKHR>& pAvailablePresentModes);
		VkExtent2D				swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);

			void			createFramebuffers();
	void			createRenderPass();
VkImageView swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);

		~Swapchain(){
			// vkDestroySwapchainKHR(dvc::graphics.LD, swapchain, nullptr);
			cleanup();
		}
	};








	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR	capabilities;
		RtArray<VkSurfaceFormatKHR>	formats;
		RtArray<VkPresentModeKHR>	presentModes;
	};
	__attribute__((used)) static SwapChainSupportDetails getSwapchainSupportDetails(const VkPhysicalDevice vDevice, const VkSurfaceKHR vSurface) {
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
