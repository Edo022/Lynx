#pragma once
#define LNX_H_GCOMMANDS
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"




namespace lnx::core::render::cmd{
	__init_var_dec(vk::CommandPool, singleTimeCommandPool);


	void              createGraphicsCommandPool();
	vk::CommandBuffer beginSingleTimeCommands();
	void              endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer);
}