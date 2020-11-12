#pragma once
#define LUX_H_CBUFFERS
#include "vulkan/vulkan.h"




namespace lux::core::c::buffers{
	void createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);
}