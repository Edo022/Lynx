#pragma once
#define LUX_H_GCOMMANDS
#include <vulkan/vulkan.hpp>
#include "LuxEngine/Types/Containers/RtArray.hpp"




namespace lux::core::render::cmd{
	extern vk::CommandPool			singleTimeCommandPool;


	void				createGraphicsCommandPool();
	vk::CommandBuffer	beginSingleTimeCommands();
	void				endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer);
}