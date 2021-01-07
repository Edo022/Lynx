#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/CBuffers.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"








namespace lux::core::c::buffers{
	//TODO use custom allocations for shared memory with those callbacks
	//FIXME add alignment
	inline void* allocateCallback(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope){
		//FIXME add function that does not initialize the data
		((ram::Alloc<char>*)pUserData)->realloc(size);
		return ((ram::Alloc<char>*)pUserData);
	}
	inline void* reallocateCallback(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope){
		// lux::ram::reallocBck(*(lux::ram::ptr<char>*)pUserData, size);
		((ram::Alloc<char>*)pUserData)->realloc(size);
		return ((ram::Alloc<char>*)pUserData);
	}
	inline void freeCallback(void* pUserData, void* pMemory){
		// lux::ram::free(*(lux::ram::ptr<char>*)pUserData);
		((ram::Alloc<char>*)pUserData)->free();
	}
	//TODO remove those functions. Theyre probably useless
	//TODO or separate gpu shared allocations from normal ones
	inline void internalAllocCallback(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope){}
	inline void internalFreeCallback (void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope){}
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
			.memoryTypeIndex = render::findMemoryType(memRequirements.memoryTypeBits, vProperties)
		};
		const VkAllocationCallbacks allocator{
			.pUserData = new ram::Alloc<char>(),
			.pfnAllocation = allocateCallback,
			.pfnReallocation = reallocateCallback,
			.pfnFree = freeCallback,
			.pfnInternalAllocation = nullptr,
			.pfnInternalFree = nullptr,
			//TODO
			// .pfnInternalAllocation,
			// .pfnInternalFree,
		};
		// switch(vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
		switch(vkAllocateMemory(vDevice, &allocInfo, &allocator, pMemory)) {
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {			//If out of dedicated memory, use the shared memory
				VkMemoryAllocateInfo allocInfo2{
					.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					.allocationSize = memRequirements.size,
					.memoryTypeIndex = render::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
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