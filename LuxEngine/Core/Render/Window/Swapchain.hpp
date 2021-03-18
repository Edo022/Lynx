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

		VkSwapchainKHR			 swapchain;
		public: //FIXME
		VkSwapchainCreateInfoKHR createInfo;	//Informations used to create the swapchain. They are saved in order to be reused when the swapchain has to be recreated
		private:
		VkRenderPass             renderPass;
		bool                     useVSync = true;


		struct SwpFrame{
			VkSemaphore s_aquired;
			VkSemaphore s_objects;
			VkSemaphore s_copy;
			VkSemaphore s_clear;
			VkFence     f_rendered;
		};
		RtArray<SwpFrame> frames;		//Synchronization objects used to render the frames
		uint32            curFrame = 0;	//The index of the frame that is currently being rendered


		struct SwpImage{
			VkImage       image;
			VkImageView   view;
			VkFramebuffer fbuffer;
		};
		RtArray<SwpImage> images;		//Images where the frames are outputted
		inline VkImageView   createImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);
		inline VkFramebuffer createFramebuffer(VkRenderPass vRenderPass, VkImageView& vAttachment, uint32 vWith, uint32 vHeight);




		VkSurfaceFormatKHR chooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>& pAvailableFormats);
		VkPresentModeKHR   choosePresentMode  (const RtArray<VkPresentModeKHR>&   pAvailablePresentModes);
		VkExtent2D         chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);

		VkSurfaceCapabilitiesKHR    getCapabilities();
		RtArray<VkSurfaceFormatKHR> getSurfaceFormats();
		RtArray<VkPresentModeKHR>   getPresentModes();




	public:
		Window* bindedWindow;				//Window owning the swapchain
		std::atomic<bool> resized = false;	//Synchronization variable. Set to true by the main engine thread when the window needs to be resized

		RtArray<LuxShader_t, uint32> shaders;
		RtArray<VkCommandBuffer>     shadersCBs;


		Swapchain();					//Object constructor. Allocates memory and initializes some resources
		void create(bool vUseVSync);	//Creates the swapchain
		void createRenderPass();		//"create" helper function

		void destroy();					//Only destroys a part of the swapchain so that it can be recreated easily
		void recreate();				//Recreates the swapchain after it has been destroyed
		~Swapchain();					//Object destructor. Deallocates memory and destroys all of the resources
	};
}
