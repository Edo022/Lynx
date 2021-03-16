#pragma once
#define LUX_H_SWAPCHAIN
#include "LuxEngine/Core/Render/Shaders/Shader_t.hpp"





namespace lux{
	struct Window;
}
namespace lux::core::wnd{
	constexpr int32 __renderMaxFramesInFlight = 2;	//Default:2




	struct Swapchain{
	private:
		friend class lux::Window;

		public: //FIXME
		VkSwapchainCreateInfoKHR createInfo;
		private:
		VkRenderPass renderPass;
		bool useVSync = true;

		RtArray<VkSemaphore> s_imageAcquired;
		RtArray<VkSemaphore> s_objectsRendered;
		RtArray<VkSemaphore> s_copy;
		RtArray<VkSemaphore> s_clear;
		RtArray<VkFence>     f_imageRendered;
		int32                curFrame = 0;		//The index of the frame that is currently being rendered


		VkSurfaceFormatKHR chooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>& pAvailableFormats);
		VkPresentModeKHR   choosePresentMode(const RtArray<VkPresentModeKHR>& pAvailablePresentModes);
		VkExtent2D         chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);

		VkSurfaceCapabilitiesKHR getCapabilities();
		RtArray<VkSurfaceFormatKHR> getSurfaceFormats();
		RtArray<VkPresentModeKHR> getPresentModes();

		VkImageView        createImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);

		VkSwapchainKHR			swapchain;
		RtArray<VkImage>		images;
		RtArray<VkImageView>	imageViews;
		RtArray<VkFramebuffer>	framebuffers;




	public:
		Window* bindedWindow;
		bool renderFramebufferResized = false;

		RtArray<LuxShader_t, uint32> shaders;
		RtArray<VkCommandBuffer>     shadersCBs;


		Swapchain();
		void create(bool vUseVSync);
		void createRenderPass();
		void createFramebuffers();

		void recreate();
		void destroy();
		~Swapchain();
	};
}
