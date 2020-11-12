#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Compute/CBuffers.hpp"
#include "LuxEngine/Core/Graphics/GCommands.hpp"








namespace lux::core::c::buffers{
	//TODO use custom allocations for shared memory with those callbacks
	void* allocateCallback(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope){
		return nullptr;
	}
	void* reallocateCallback(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope){
		return nullptr;
	}
	void freeCallback(void* pUserData, void* pMemory){
	}
	void internalAllocCallback(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope){
	}
	void internalFreeCallback(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope){
	}
	//PFN_vkFreeFunction








	//Creates and allocates a buffer in the memory of a device
	//*   vDevice: the logical device where to create the buffer
	//*   vSize: the count of the buffer in bytes
	//*   vUsage: flags defining the usage of the buffer (VK_BUFFER_USAGE...)
	//*   vProperties: flags defining the properties of the memory (VK_MEMORY_PROPERTY_...)
	//*   pBuffer: the buffer object to allocate
	//*   pMemory: the memory of the buffer
	void createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory) {
		VkBufferCreateInfo bufferInfo{ };
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = vSize;
		bufferInfo.usage = vUsage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		TryVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer)) printError("Failed to create buffer", true, -1);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, vProperties)
		};
		VkAllocationCallbacks allocator{
			.pUserData = nullptr,
			.pfnAllocation = allocateCallback,
			.pfnReallocation =reallocateCallback,
			.pfnFree = freeCallback,
			.pfnInternalAllocation = internalAllocCallback,
			.pfnInternalFree =internalFreeCallback,
		};
		switch(vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {			//If out of dedicated memory, use the shared memory
				VkMemoryAllocateInfo allocInfo2{ };
				allocInfo2.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo2.allocationSize = memRequirements.size;
				allocInfo2.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr, pMemory)) {
					case VK_SUCCESS: break;
					case VK_ERROR_OUT_OF_HOST_MEMORY: goto CaseOutOfHostMemory;
					default: printError("Failed to allocate buffer memory", true, -1);
				}
				break;
			}
			case VK_ERROR_OUT_OF_HOST_MEMORY: { //TODO free cells when there is not much memory left
				lux_error(true, "Vulkan error: Out of host memory");
				break;
			}
			case VK_ERROR_TOO_MANY_OBJECTS: {
				CaseOutOfHostMemory:
				lux_error(true, "Vulkan error: Too many objects. This error is caused by the engine. Contact the developer. He thought this couldn't happen, but somehow it did");
				break;
			}
			default: printError("Failed to allocate buffer memory", true, -1);
		}

		TryVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0)) printError("Failed to bind buffer", true, -1);
	}
}