
#pragma once
#include "LuxEngine/Core/Compute/CShader_t.h"
#define LUX_H_COMPUTE




namespace lux::core::c{
	extern VkCommandPool			copyCommandPool;
	extern DynArray<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer			clearCommandBuffer;

	void preInit( );
	void init( );
	void computeCleanup( );
}