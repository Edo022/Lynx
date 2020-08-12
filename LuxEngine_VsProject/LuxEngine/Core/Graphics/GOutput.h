
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.h"



namespace lux::core::g::out{
	extern VkRenderPass		renderPass;
	extern const int32		renderMaxFramesInFlight;	//Default:2
	extern bool				renderFramebufferResized;	//Updates the swapchain when the window is resized

	void			createRenderPass( );
	void			createFramebuffers( );
	VkImageView		swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);
}
