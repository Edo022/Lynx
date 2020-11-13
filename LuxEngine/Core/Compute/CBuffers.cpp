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
		VkBufferCreateInfo bufferInfo{
			.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size			= vSize,
			.usage			= vUsage,
			.sharingMode	= VK_SHARING_MODE_EXCLUSIVE
		};
		luxCheckVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer), "Failed to create buffer");

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
				VkMemoryAllocateInfo allocInfo2{
					.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					.allocationSize = memRequirements.size,
					.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				};
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr, pMemory)) {
					case VK_SUCCESS: break;
					case VK_ERROR_OUT_OF_HOST_MEMORY: goto CaseOutOfHostMemory;
					default: luxPrintError("Failed to allocate buffer memory");
				}
				break;
			}
			case VK_ERROR_OUT_OF_HOST_MEMORY: { //TODO free cells when there is not much memory left
				luxPrintError("Vulkan error: Out of host memory");
				break;
			}
			case VK_ERROR_TOO_MANY_OBJECTS: {
				CaseOutOfHostMemory:
				luxPrintError("Vulkan error: Too many objects. This error is caused by the engine. Contact the developer. He thought this couldn't happen, but somehow it did");
				break;
			}
			default: luxPrintError("Failed to allocate buffer memory");
		}

		luxCheckVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0), "Failed to bind buffer");
	}
}