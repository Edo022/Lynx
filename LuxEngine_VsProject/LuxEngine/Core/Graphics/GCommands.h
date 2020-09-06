
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#define LUX_H_GCOMMANDS




namespace lux::core::g::cmd{
	extern VkCommandPool			singleTimeCommandPool;
	extern Array<VkCommandBuffer>	singleTimeCommandBuffers;


	void preInit( );
	void				createGraphicsCommandPool( );
	VkCommandBuffer		beginSingleTimeCommands( );
	void				endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);
}