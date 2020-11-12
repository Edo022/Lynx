
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.hpp"
#define LUX_H_GOUTPUT




namespace lux::core::g::out{
	extern VkRenderPass		renderPass;
	constexpr int32			renderMaxFramesInFlight = 2;	//Default:2
	extern bool				renderFramebufferResized;		//Updates the swapchain when the window is resized


	void preInit( );
	void			createRenderPass( );
	void			createFramebuffers( );
	VkImageView		swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);
}
