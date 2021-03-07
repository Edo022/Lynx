#pragma once
#define LUX_H_CBUFFERS
#include <vulkan/vulkan.h>




namespace lux::core::buffers{
	// extern VkCommandPool			copyCommandPool;
	// extern RtArray<VkCommandBuffer>	copyCommandBuffers;
	// extern VkCommandBuffer			clearCommandBuffer;

	// void init();
	void createBuffer(VkBuffer* pBuffer, const VkBufferUsageFlags vUsage, const VkDeviceSize vSize, VkDeviceMemory* pMemory, const VkMemoryPropertyFlags vProperties, const VkDevice vDevice);
	void cleanup();
}