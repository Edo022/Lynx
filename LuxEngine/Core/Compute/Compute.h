#pragma once
#define LUX_H_COMPUTE
#include "LuxEngine/Core/Compute/CShader_t.h"




namespace lux::core::c{
	extern VkCommandPool				copyCommandPool;
	extern DynArray<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer				clearCommandBuffer;

	void preInit( );
	void init( );
	void computeCleanup( );
}