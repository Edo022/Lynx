#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/GSwapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"







namespace lux::core::buffers{
	alignCache VkCommandPool			copyCommandPool    = nullptr;
	alignCache RtArray<VkCommandBuffer>	copyCommandBuffers;
	alignCache VkCommandBuffer			clearCommandBuffer = nullptr;




	//Initializes the compute objects
	//> Engine internal use
	void init() {
		{ //Initialize window buffers and count
			lux::window.gpuCellWindowOutput_i.realloc(lux::window.width * lux::window.height * 4); //A8-R8-G8-B8 UI
			lux::window.gpuCellWindowOutput.  realloc(lux::window.width * lux::window.height * 4 * 4); //A32-R32-G32-B32 UF
			lux::window.gpuCellWindowZBuffer. realloc(lux::window.width * lux::window.height * 4);
			lux::window.gpuCellWindowSize.    realloc(4 * 2);	//Create cell for window size //TODO use dedicated storage and update every time

			lux::window.gpuCellWindowSize.map();			//Map window size cell
			lux::window.gpuCellWindowSize[0] = core::render::swapchain::swapchainExtent.width;				//Set width
			lux::window.gpuCellWindowSize[1] = core::render::swapchain::swapchainExtent.height;				//Set height
			lux::window.gpuCellWindowSize.unmap();										//Unmap
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(render::swapchain::swapchainImages.count());			//Resize the command buffer array in the shader
			core::c::shaders::createDefaultCommandBuffers();								//Create command buffers and command pool
		}
	}








	//TODO use user defined callbacks
	// inline void* allocateCallback(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {}
	// inline void* reallocateCallback(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {}
	// inline void freeCallback(void* pUserData, void* pMemory) {}
	// inline void internalAllocCallback(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {}
	// inline void internalFreeCallback (void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {}




	/**
	 * @brief Allocates a buffer in the VRAM of a device
	 * @param pBuffer The buffer object to allocate
	 * @param vUsage Vulkan buffer usage flags
	 * @param vSize The size in bytes of the allocation
	 * @param pMemory The memory of the buffer
	 * @param vProperties Vulkan memory properties flags
	 * @param vDevice The logical device where to allocate the buffer
	 */
	void createBuffer(VkBuffer* pBuffer, const VkBufferUsageFlags vUsage, const VkDeviceSize vSize, VkDeviceMemory* pMemory, const VkMemoryPropertyFlags vProperties, const VkDevice vDevice) {
		VkBufferCreateInfo bufferInfo{
			.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size        = vSize,
			.usage       = vUsage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};
		dbg::checkVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer), "Failed to create buffer");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = render::findMemoryType(memRequirements.memoryTypeBits, vProperties)
		};
		//TODO USE CUSTOM ALLOCATOR
		// const VkAllocationCallbacks allocator{
		// 	.pUserData = new ram::ptr<char>(),
		// 	.pfnAllocation = allocateCallback,
		// 	.pfnReallocation = reallocateCallback,
		// 	.pfnFree = freeCallback,
		// 	.pfnInternalAllocation = internalAllocCallback,
		// 	.pfnInternalFree = internalFreeCallback,
		// };
		switch(vkAllocateMemory(vDevice, &allocInfo, nullptr/*, new VkAllocationCallbacks(allocator)*/, pMemory)) { //TODO
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {			//If out of dedicated memory, use the shared memory
				VkMemoryAllocateInfo allocInfo2{
					.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					.allocationSize = memRequirements.size,
					.memoryTypeIndex = render::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				};
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr/*, new VkAllocationCallbacks(allocator)*/, pMemory)) { //TODO
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








	//Frees and destroys the compute objects //TODO fix
	//> Engine internal use
	void cleanup() {
		// for(uint32 i = 0; i < /*vram::buffers.count()*/(uint32)lux::__pvt::CellClassIndex::NUM * (uint32)lux::AllocType::NUM; ++i) {
		// 	for(uint32 j = 0; j < vram::buffers[i].buffers.count(); ++j) {
		// 		if(vram::buffers[i].buffers.isValid(j)) {
		// 			vkDestroyBuffer(dvc::compute.LD, vram::buffers[i].buffers[j].buffer, nullptr);
		// 			vkFreeMemory(dvc::compute.LD, vram::buffers[i].buffers[j].memory, nullptr);
		// 		}
		// 	}
		// }
	}
}