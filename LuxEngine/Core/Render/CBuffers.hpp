#pragma once
#define LUX_H_CBUFFERS
#include "vulkan/vulkan.h"




namespace lux::core::c::buffers{
	void createBuffer(VkBuffer* pBuffer, const VkBufferUsageFlags vUsage, const VkDeviceSize vSize, VkDeviceMemory* pMemory, const VkMemoryPropertyFlags vProperties, const VkDevice vDevice);
}