
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/LuxArray.h"




namespace lux::core::g::cmd{
	extern VkCommandPool			singleTimeCommandPool;
	extern Array<VkCommandBuffer>	singleTimeCommandBuffers;

	void				createGraphicsCommandPool( );
	VkCommandBuffer		beginSingleTimeCommands( );
	void				endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);
}