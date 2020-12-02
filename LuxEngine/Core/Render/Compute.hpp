#pragma once
#define LUX_H_COMPUTE
#include "LuxEngine/Core/Render/CShader_t.hpp"




namespace lux::core::c{
	extern VkCommandPool			copyCommandPool;
	extern RtArray<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer			clearCommandBuffer;

	void preInit( );
	void init( );
	void computeCleanup( );
}