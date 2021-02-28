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
			// render::wnd::gpuCellWindowOutput_i	= vram::allocBck(render::wnd::width * render::wnd::height * 4, 	  CellClass::AUTO, lux::AllocType::VRamStorage); //A8-R8-G8-B8 UI
			// render::wnd::gpuCellWindowOutput	= vram::allocBck(render::wnd::width * render::wnd::height * 4 * 4, CellClass::AUTO, lux::AllocType::VRamStorage); //A32-R32-G32-B32 UF
			// render::wnd::gpuCellWindowZBuffer 	= vram::allocBck(render::wnd::width * render::wnd::height * 4, 	  CellClass::AUTO, lux::AllocType::VRamStorage);
			// render::wnd::gpuCellWindowSize = vram::allocBck(4 * 2,  CellClass::AUTO, lux::AllocType::RamStorage);	//Create cell for window size //TODO use dedicated storage and update every time
			render::wnd::gpuCellWindowOutput_i.alloc_(render::wnd::width * render::wnd::height * 4,     vram::VCellClass::CLASS_A); //A8-R8-G8-B8 UI
			render::wnd::gpuCellWindowOutput.  alloc_(render::wnd::width * render::wnd::height * 4 * 4, vram::VCellClass::CLASS_A); //A32-R32-G32-B32 UF
			render::wnd::gpuCellWindowZBuffer. alloc_(render::wnd::width * render::wnd::height * 4,     vram::VCellClass::CLASS_A);
			render::wnd::gpuCellWindowSize.alloc_(4 * 2, vram::VCellClass::CLASS_A);	//Create cell for window size //TODO use dedicated storage and update every time

			//FIXME ADD MAP AND UNMAP METHODS
			// uint32* pwindowSize = (uint32*)(render::wnd::gpuCellWindowSize->map());			//Map window size cell //TODO use gpu pointer instead of raw cell
			// pwindowSize[0] = core::render::swapchain::swapchainExtent.width;				//Set width
			// pwindowSize[1] = core::render::swapchain::swapchainExtent.height;				//Set height
			// render::wnd::gpuCellWindowSize->unmap();										//Unmap
			render::wnd::gpuCellWindowSize.map();			//Map window size cell //TODO use gpu pointer instead of raw cell
			render::wnd::gpuCellWindowSize[0] = core::render::swapchain::swapchainExtent.width;				//Set width
			render::wnd::gpuCellWindowSize[1] = core::render::swapchain::swapchainExtent.height;				//Set height
			render::wnd::gpuCellWindowSize.unmap();										//Unmap
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(render::swapchain::swapchainImages.count( ));			//Resize the command buffer array in the shader
			core::c::shaders::createDefaultCommandBuffers( );								//Create command buffers and command pool
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
		// for(uint32 i = 0; i < /*vram::buffers.count( )*/(uint32)lux::__pvt::CellClassIndex::NUM * (uint32)lux::AllocType::NUM; ++i) {
		// 	for(uint32 j = 0; j < vram::buffers[i].buffers.count( ); ++j) {
		// 		if(vram::buffers[i].buffers.isValid(j)) {
		// 			vkDestroyBuffer(dvc::compute.LD, vram::buffers[i].buffers[j].buffer, nullptr);
		// 			vkFreeMemory(dvc::compute.LD, vram::buffers[i].buffers[j].memory, nullptr);
		// 		}
		// 	}
		// }
	}
}