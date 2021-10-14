#pragma once
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"




namespace lnx::core::render::cmd{
	_lnx_init_var_value_dec((vk::CommandPool), singleTimeCommandPool);
	_lnx_init_fun_dec(LNX_H_GCOMMANDS);

	void              createGraphicsCommandPool();
	vk::CommandBuffer beginSingleTimeCommands();
	void              endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer);
}