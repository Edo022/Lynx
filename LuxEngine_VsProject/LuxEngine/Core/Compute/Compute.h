
#pragma once
#include "LuxEngine/Core/Compute/CShader_t.h"
#define LUX_H_COMPUTE




namespace lux::core::c{
	extern  VkCommandPool			copyCommandPool;
	extern Array<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer			clearCommandBuffer;

	void preInit( );
	void init( );
	void computeCleanup( );
}