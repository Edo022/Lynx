
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#define LUX_H_GCOMMANDS
//#include "LuxEngine/Core/Core.h"




namespace lux::core::g::cmd{
	extern VkCommandPool			singleTimeCommandPool;
	extern Array<VkCommandBuffer>	singleTimeCommandBuffers;
	//PostInitializerHeader(LUX_H_GCOMMANDS);


	void init( );
	void				createGraphicsCommandPool( );
	VkCommandBuffer		beginSingleTimeCommands( );
	void				endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);
}