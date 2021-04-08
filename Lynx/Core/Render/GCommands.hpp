#pragma once
#define LNX_H_GCOMMANDS
#include <vulkan/vulkan.hpp>
#include "Lynx/Types/Containers/RtArray.hpp"




namespace lnx::core::render::cmd{
	extern vk::CommandPool			singleTimeCommandPool;


	void				createGraphicsCommandPool();
	vk::CommandBuffer	beginSingleTimeCommands();
	void				endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer);
}