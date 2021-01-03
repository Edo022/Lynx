#pragma once
#define LUX_H_GCOMMANDS
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/RtArray.hpp"




namespace lux::core::render::cmd{
	extern VkCommandPool			singleTimeCommandPool;
	extern RtArray<VkCommandBuffer>	singleTimeCommandBuffers;


	void preInit( );
	void				createGraphicsCommandPool( );
	VkCommandBuffer		beginSingleTimeCommands( );
	void				endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);
}