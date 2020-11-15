#pragma once
#define LUX_H_GCOMMANDS
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/RTArray.hpp"




namespace lux::core::g::cmd{
	extern VkCommandPool			singleTimeCommandPool;
	extern RTArray<VkCommandBuffer>	singleTimeCommandBuffers;


	void preInit( );
	void				createGraphicsCommandPool( );
	VkCommandBuffer		beginSingleTimeCommands( );
	void				endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);
}