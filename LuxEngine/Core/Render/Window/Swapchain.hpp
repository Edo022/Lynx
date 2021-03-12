#pragma once
#define LUX_H_SWAPCHAIN
#include "LuxEngine/Core/Render/Shaders/Shader_t.hpp"





namespace lux{
	struct Window;
}
namespace lux::core::wnd{
	constexpr int32 __renderMaxFramesInFlight = 2;	//Default:2


	struct Swapchain{
		VkSwapchainKHR			swapchain;
		RtArray<VkImage>		images;
		RtArray<VkImageView>	imageViews;
		VkFormat				imageFormat;
		VkExtent2D				swapchainExtent;
		RtArray<VkFramebuffer>	framebuffers;

		Window* bindedWindow;
		VkRenderPass renderPass;
		bool renderFramebufferResized = false;
		bool useVSync = true;

		RtArray<VkSemaphore> s_imageAcquired;
		RtArray<VkSemaphore> s_objectsRendered;
		RtArray<VkSemaphore> s_copy;
		RtArray<VkSemaphore> s_clear;
		RtArray<VkFence>     f_imageRendered;
		int32                renderCurrentFrame = 0;

		RtArray<LuxShader_t, uint32> shaders;
		RtArray<VkCommandBuffer>     shadersCBs;




		Swapchain();
		void create(bool vUseVSync);
		void createRenderPass();
		void createFramebuffers();
		VkImageView        createImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);
		VkSurfaceFormatKHR chooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>& pAvailableFormats);
		VkPresentModeKHR   choosePresentMode(const RtArray<VkPresentModeKHR>& pAvailablePresentModes);
		VkExtent2D         chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);

		// void recreate(const bool vWindowResized);
		void recreate();
		void destroy();
		~Swapchain();




		struct Details {
			VkSurfaceCapabilitiesKHR	capabilities;
			RtArray<VkSurfaceFormatKHR>	formats;
			RtArray<VkPresentModeKHR>	presentModes;
		};
		__attribute__((used)) static Details getDetails(const VkPhysicalDevice vDevice, const VkSurfaceKHR vSurface) {
			Details details;

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
	};
}
