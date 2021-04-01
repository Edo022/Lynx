#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/Core/Render/Render.hpp"







namespace lux::core::buffers{
	/**
	 * @brief Allocates a buffer in the VRAM of a device
	 * @param pBuffer The buffer object to allocate
	 * @param vUsage Vulkan buffer usage flags
	 * @param vSize The size in bytes of the allocation
	 * @param pMemory The memory of the buffer
	 * @param vProperties Vulkan memory properties flags
	 * @param vDevice The logical device where to allocate the buffer
	 */
	void createBuffer(vk::Buffer* pBuffer, const vk::BufferUsageFlags vUsage, const vk::DeviceSize vSize, vk::DeviceMemory* pMemory, const vk::MemoryPropertyFlags vProperties, const vk::Device vDevice) {
		vk::BufferCreateInfo bufferInfo{
			.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size        = vSize,
			.usage       = vUsage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};
		dbg::checkVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer), "Failed to create buffer");

		vk::MemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

		vk::MemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = render::findMemoryType(memRequirements.memoryTypeBits, vProperties)
		};
		//TODO USE CUSTOM ALLOCATOR
		// const vk::AllocationCallbacks allocator{
		// 	.pUserData = new ram::ptr<char>(),
		// 	.pfnAllocation = allocateCallback,
		// 	.pfnReallocation = reallocateCallback,
		// 	.pfnFree = freeCallback,
		// 	.pfnInternalAllocation = internalAllocCallback,
		// 	.pfnInternalFree = internalFreeCallback,
		// };
		switch(vkAllocateMemory(vDevice, &allocInfo, nullptr/*, new vk::AllocationCallbacks(allocator)*/, pMemory)) { //TODO
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {			//If out of dedicated memory, use the shared memory
				vk::MemoryAllocateInfo allocInfo2{
					.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					.allocationSize = memRequirements.size,
					.memoryTypeIndex = render::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				};
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr/*, new vk::AllocationCallbacks(allocator)*/, pMemory)) { //TODO
					case VK_SUCCESS: break;
					case VK_ERROR_OUT_OF_HOST_MEMORY: goto CaseOutOfHostMemory;
					default: goto CaseAllocFailure;
				}
				break;
			}
			case VK_ERROR_OUT_OF_HOST_MEMORY: {
				CaseOutOfHostMemory:
				dbg::printError("Vulkan error: Out of host memory");
				break;
			}
			case VK_ERROR_TOO_MANY_OBJECTS: {
				dbg::printError("Vulkan error: Too many objects. This error is caused by the engine. Contact the developer");
				break;
			}
			default: CaseAllocFailure: dbg::printError("Failed to allocate buffer memory");
		}

		dbg::checkVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0), "Failed to bind buffer");
	}
}