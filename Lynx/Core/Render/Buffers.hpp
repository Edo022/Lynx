#pragma once
#define LNX_H_CBUFFERS
#include <vulkan/vulkan.hpp>




namespace lnx::core::buffers{
	void createBuffer(vk::Buffer* pBuffer, const vk::BufferUsageFlags vUsage, const vk::DeviceSize vSize, vk::DeviceMemory* pMemory, const vk::MemoryPropertyFlags vProperties, const vk::Device vDevice);
}