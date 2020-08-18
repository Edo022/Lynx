
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/EngineTypes.h"




namespace lux::core::c::buffers{
	void init( );



	void createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);
	void copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize);
}