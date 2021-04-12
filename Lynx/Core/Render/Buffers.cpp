#include "Lynx/Core/Render/Buffers.hpp"
#include "Lynx/Core/Render/Render.hpp"







namespace lnx::core::buffers{
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
		auto bufferInfo = vk::BufferCreateInfo()
			.setSize        (vSize)
			.setUsage       (vUsage)
			.setSharingMode (vk::SharingMode::eExclusive)
		;
		switch(vDevice.createBuffer(&bufferInfo, nullptr, pBuffer)){
			case vk::Result::eErrorInvalidOpaqueCaptureAddress: dbg::printError("Invalid opaque capture address"); break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
			case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		}


		vk::MemoryRequirements memRequirements;
		vDevice.getBufferMemoryRequirements(*pBuffer, &memRequirements);

		auto allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize  (memRequirements.size)
			.setMemoryTypeIndex (render::findMemoryType(memRequirements.memoryTypeBits, vProperties))
		;
		//TODO USE CUSTOM ALLOCATOR
		// const vk::AllocationCallbacks allocator{
		// 	.pUserData = new ram::ptr<char>(),
		// 	.pfnAllocation = allocateCallback,
		// 	.pfnReallocation = reallocateCallback,
		// 	.pfnFree = freeCallback,
		// 	.pfnInternalAllocation = internalAllocCallback,
		// 	.pfnInternalFree = internalFreeCallback,
		// };
		switch(vDevice.allocateMemory(&allocInfo, nullptr/*, new vk::AllocationCallbacks(allocator)*/, pMemory)) { //TODO
			case vk::Result::eSuccess : break;
			case vk::Result::eErrorOutOfDeviceMemory: {			//If out of dedicated memory, use the shared memory
				vk::MemoryAllocateInfo allocInfo2 = vk::MemoryAllocateInfo()
					.setAllocationSize  (memRequirements.size)
					.setMemoryTypeIndex (render::findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostCached | vk::MemoryPropertyFlagBits::eHostVisible))
				;
				switch(vDevice.allocateMemory(&allocInfo2, nullptr/*, new vk::AllocationCallbacks(allocator)*/, pMemory)) { //TODO
					case vk::Result::eSuccess: break;
					case vk::Result::eErrorOutOfHostMemory: goto CaseOutOfHostMemory;
					default: goto CaseAllocFailure;
				}
				break;
			}
			case vk::Result::eErrorOutOfHostMemory: {
				CaseOutOfHostMemory:
				dbg::printError("Vulkan error: Out of host memory");
				break;
			}
			case vk::Result::eErrorTooManyObjects: {
				dbg::printError("Vulkan error: Too many objects. This error is caused by the engine. Contact the developer");
				break;
			}
			default: CaseAllocFailure: dbg::printError("Failed to allocate buffer memory");
		}

		vDevice.bindBufferMemory(*pBuffer, *pMemory, 0);
	}
}