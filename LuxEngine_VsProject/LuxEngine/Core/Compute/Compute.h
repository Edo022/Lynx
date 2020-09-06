
#pragma once
#include "LuxEngine/Core/Compute/CShader_t.h"
#define LUX_H_COMPUTE
//#include "LuxEngine/Core/Core.h"


namespace lux::core::c{
	constexpr VkCommandPool getNullptr( ){ return nullptr; };

	extern  VkCommandPool			copyCommandPool;
	extern Array<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer			clearCommandBuffer;
	//PostInitializerHeader(LUX_H_COMPUTE);

	void _init( );
	void init( );
	void computeCleanup( );
}