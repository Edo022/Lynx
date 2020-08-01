#pragma once
//#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/CShader_t.h"



namespace lux::core::c{
	extern VkCommandPool			copyCommandPool;
	extern Array<VkCommandBuffer>	copyCommandBuffers;
	extern VkCommandBuffer			clearCommandBuffer;

	void computeInit();
	void computeCleanup();
}