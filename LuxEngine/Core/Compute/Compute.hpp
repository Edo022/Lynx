#pragma once
#define LUX_H_COMPUTE
#include "LuxEngine/Core/Compute/CShader_t.hpp"




namespace lux::core::c{
	extern VkCommandPool				copyCommandPool;
	extern RTArray<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer				clearCommandBuffer;

	void preInit( );
	void init( );
	void computeCleanup( );
}