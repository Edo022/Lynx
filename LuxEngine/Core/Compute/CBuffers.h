#pragma once
#include "vulkan/vulkan.h"
#define LUX_H_CBUFFERS



namespace lux::core::c::buffers{
	void createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);
}