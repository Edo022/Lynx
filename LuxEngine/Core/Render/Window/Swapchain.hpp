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

		vk::SwapchainKHR			 swapchain;
		public: //FIXME
		vk::SwapchainCreateInfoKHR createInfo;	//Informations used to create the swapchain. They are saved in order to be reused when the swapchain has to be recreated
		private:
		vk::RenderPass             renderPass;
		bool                     useVSync = true;


		struct SwpFrame{
			vk::Semaphore s_aquired;
			vk::Semaphore s_objects;
			vk::Semaphore s_copy;
			vk::Semaphore s_clear;
			vk::Fence     f_rendered;
		};
		RtArray<SwpFrame> frames;		//Synchronization objects used to render the frames
		uint32            curFrame = 0;	//The index of the frame that is currently being rendered


		struct SwpImage{
			vk::Image       image;
			vk::ImageView   view;
			vk::Framebuffer fbuffer;
		};
		RtArray<SwpImage> images;		//Images where the frames are outputted
		inline vk::ImageView   createImageView(const vk::Image vImage, const vk::Format vFormat, const vk::ImageAspectFlags vAspectFlags);
		inline vk::Framebuffer createFramebuffer(vk::RenderPass vRenderPass, vk::ImageView& vAttachment, uint32 vWith, uint32 vHeight);




		vk::SurfaceFormatKHR chooseSurfaceFormat  (const RtArray<vk::SurfaceFormatKHR>& pAvailableFormats);
		vk::PresentModeKHR   choosePresentMode    (const RtArray<vk::PresentModeKHR>&   pAvailablePresentModes);
		vk::Extent2D         chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR* pCapabilities);

		vk::SurfaceCapabilitiesKHR    getCapabilities();
		RtArray<vk::SurfaceFormatKHR> getSurfaceFormats();
		RtArray<vk::PresentModeKHR>   getPresentModes();




	public:
		Window* bindedWindow;				//Window owning the swapchain
		std::atomic<bool> resized = false;	//Synchronization variable. Set to true by the main engine thread when the window needs to be resized

		RtArray<vk::CommandBuffer>     shadersCBs;


		Swapchain();					//Object constructor. Allocates memory and initializes some resources
		void create(bool vUseVSync);	//Creates the swapchain
		void createRenderPass();		//"create" helper function

		void destroy();					//Only destroys a part of the swapchain so that it can be recreated easily
		void recreate();				//Recreates the swapchain after it has been destroyed
		~Swapchain();					//Object destructor. Deallocates memory and destroys all of the resources
	};
}
