

#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Devices.h"







namespace lux::core::c{
	Map<LuxBuffer_t, uint32> CBuffers; //List of GPU buffers








	//Creates and allocates a buffer in the memory of a device
//*   vDevice: the logical device where to create the buffer
//*   vSize: the size of the buffer in bytes
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
		TryVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer)) Exit("Failed to create buffer");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{ };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = lux::core::g::graphicsFindMemoryType(memRequirements.memoryTypeBits, vProperties);

		//TODO check out of memory
		//TODO don't quit in VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS but return an error code
		switch(vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {	//IF out of device memory, use the host memory
				VkMemoryAllocateInfo allocInfo2{ };
				allocInfo2.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo2.allocationSize = memRequirements.size;
				allocInfo2.memoryTypeIndex = lux::core::g::graphicsFindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr, pMemory)) {
					case VK_SUCCESS: break;
					case VK_ERROR_OUT_OF_HOST_MEMORY: //TODO add case. same as next out of host memory
					default: Exit("Failed to allocate buffer memory");
				}
				break;
			}
			case VK_ERROR_OUT_OF_HOST_MEMORY:		//TODO If out of host memory
			case VK_ERROR_TOO_MANY_OBJECTS:	//TODO
			default: Exit("Failed to allocate buffer memory");
		}

		TryVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0)) Exit("Failed to bind buffer");
	}




	//TODO remove if not used
	//Creates and submits a command buffer to copy from vSrcBuffer to dstBuffer
	//*   vSrcBuffer: the source buffer where to read the data
	//*   vDstBuffer: the destination buffer where to copy the data
	//*   vSize: the size in bytes of the data to copy
	void copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize) {
		VkBufferCopy copyRegion{ };												//Create buffer copy object
		copyRegion.size = vSize;												//Set size of the copied region
		//TODO add offset and automatize cells
		//copyRegion.dstOffset
		VkCommandBuffer commandBuffer = lux::core::g::beginSingleTimeCommands( );				//Start command buffer
		vkCmdCopyBuffer(commandBuffer, vSrcBuffer, vDstBuffer, 1, &copyRegion);	//Record the copy command
		lux::core::g::endSingleTimeCommands(commandBuffer);									//End command buffer
	}








	//TODO use unifom buffer with small cells
	//Creates a memory buffer in a compute device and saves it in the lux::Array "computeBuffers"
	//*   vSize          | the size in bytes of the buffer
	//*   vBufferClass   | the class of the buffer
	//*   vCpuAccessible | whether or not the CPU can access the buffer. Non accessible memory is faster but cannot be mapped
	//*       Trying to map a non accessible cell will result in an access violation error
	//			TODO fix
	//*   Returns        | the index of the created buffer. -1 if an error occurs
	LuxBuffer gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible) {
		LuxBuffer_t buffer(vSize, vBufferClass, vCpuAccessible);	//Create the buffer struct
		createBuffer(												//Create the vkBuffer as a storage buffer with transfer source and destination capabilities
			lux::core::g::compute.LD, vSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			(vCpuAccessible) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&buffer.buffer, &buffer.memory
		);
		return lux::core::c::CBuffers.add(buffer);								//Add it to the buffer array and return its index
	}




	//Creates a memory cell into a buffer. Allocates a new buffer if there are no compatible buffers with free cells
	//*   vCellSize      | the size in bytes of the cell
	//*   vCpuAccessible | whether the CPU can access the cell or not. Non accessible memory is faster but it cannot be mapped
	//*   Returns        | the code of the cell. -1 if an error occurs
	LuxCell gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible) {
		LuxBufferClass bufferClass;																		//Create a variable that stores the class of the buffer
		if(vCellSize <= LUX_BUFFER_CLASS_50) bufferClass = LUX_BUFFER_CLASS_50;							//Find the required buffer class. A cell must have a size smaller than or equal to the size of the class to belong to it
		else if(vCellSize <= LUX_BUFFER_CLASS_5K) bufferClass = LUX_BUFFER_CLASS_5K;
		else if(vCellSize <= LUX_BUFFER_CLASS_500K) bufferClass = LUX_BUFFER_CLASS_500K;
		else if(vCellSize <= LUX_BUFFER_CLASS_2MLN) bufferClass = LUX_BUFFER_CLASS_2MLN;
		else bufferClass = LUX_BUFFER_CLASS_LRG;

		if(bufferClass != LUX_BUFFER_CLASS_LRG) {														//If it's a static buffer
			LuxBuffer buffer = -1;																			//Initialize the buffer variable. It stores the index of the buffer where the cell will be created
			for(uint32 i = 0; i < lux::core::c::CBuffers.size( ); ++i) {													//Find the buffer. For each of the preexistent buffers
				if(lux::core::c::CBuffers.isValid(i) &&																		//It can be used
					lux::core::c::CBuffers[i].cpuAccessible == vCpuAccessible &&												//It's of the same memory type,
					lux::core::c::CBuffers[i].bufferClass == bufferClass &&													//If it's of the right class,
					lux::core::c::CBuffers[i].cells.usedSize( ) < LUX_CNF_GPU_STATIC_BUFFER_SIZE / lux::core::c::CBuffers[i].bufferClass) {	//And it has free cells,
					buffer = i;																					//Save its index
				}
			}
			if(buffer == (LuxBuffer)-1) buffer = gpuBufferCreate(LUX_CNF_GPU_STATIC_BUFFER_SIZE, bufferClass, vCpuAccessible);	//If no buffer was found, create a new one with the specified class and a size equal to the static buffer default size and save its index
			return createLuxCell(true, buffer, lux::core::c::CBuffers[buffer].cells.add(1), bufferClass);										//If there is buffer of the same type, create a new cell in it and return its code
		}
		else return createLuxCell(false, gpuBufferCreate(vCellSize, LUX_BUFFER_CLASS_LRG, vCpuAccessible), 0, vCellSize);	//If it's a custom size buffer, create a new buffer and return its code
	}




	//TODO choose device
	//Removes a memory cell from the cell list, destroys the vulkan object and frees its memory
	//*   vCell   | the LuxCell to destroy
	//*   returns | true if the operation succeeded, false if the cell is invalid
	bool gpuCellDestroy(const LuxCell vCell) {
		LuxBuffer buffer = getBufferIndex(vCell);								//Get buffer index
		//TODO Use lux output console
		if(!lux::core::c::CBuffers.isValid(buffer)) {
			Failure printf("Something went wrong .-.");
			Failure printf("you were trying to use an invalid index buffer %u", buffer);
			return false;								//If the buffer index is not valid, return false
		}

		if(lux::core::c::CBuffers.isValid(buffer)) {												//If it's not removed
			if(lux::core::c::CBuffers[buffer].bufferClass == LUX_BUFFER_CLASS_LRG) {					//And the buffer is a custom size buffer
				destroyBuffer://TODO destroy the old if there is one (rm if n+1 fb)
				//TODO Use lux output console
				if(!lux::core::c::CBuffers.isValid(buffer)) {
					Failure printf("Something went wrong .-.");
					Failure printf("you were trying to use an invalid index buffer %u", buffer);
					return false;									//If the buffer index is not valid, return false
				}
				else{
					vkDestroyBuffer(lux::core::g::graphics.LD, lux::core::c::CBuffers[buffer].buffer, nullptr);				//destroy the GPU buffer structure
					vkFreeMemory(lux::core::g::graphics.LD, lux::core::c::CBuffers[buffer].memory, nullptr);				//Free the buffer's memory
					lux::core::c::CBuffers.remove(buffer, false);												//Remove the buffer from the buffer array
					return true;
				}
			}
			else {																		//If it's a fixed size buffer
				//TODO Use lux output console
				if(lux::core::c::CBuffers[buffer].cells.remove(getCellIndex(vCell))) {
					Failure printf("Something went wrong .-.");
					Failure printf("you were trying to use an invalid cell index %u", getCellIndex(vCell));
					return false;		//And the cell index is invalid, return false
				}
				else{
					if(lux::core::c::CBuffers[buffer].cells.usedSize( ) == 0) goto destroyBuffer;				//If it's valid, remove the cell. If there are no cells left, destroy the buffer
					//TODO idk
					else Exit("Something went wrong .-.");
				}
			}
		}
		else return false;
	}




	//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
	//Mapping an already mapped buffer will overwrite the old mapping
	//*   buffer  | a pointer to a LuxBuffer_t object. It's the buffer that will be mapped
	//*   returns | the void pointer that maps the buffer, nullptr if an error occurs
	void* gpuCellMap(const LuxCell vCell) {
		LuxBuffer buffer = getBufferIndex(vCell);														//Get the buffer index
		//TODO Use lux output console
		if(!lux::core::c::CBuffers.isValid(buffer)) {
			Failure printf("Something went wrong .-.");
			Failure printf("you were trying to use an invalid index buffer %u", buffer);
			return nullptr;													//If the buffer index is not valid, return nullptr
		}

		if(lux::core::c::CBuffers[buffer].isMapped) vkUnmapMemory(lux::core::g::compute.LD, lux::core::c::CBuffers[buffer].memory);				//If it's already mapped, unmap the shared memory
		else lux::core::c::CBuffers[buffer].isMapped = true;															//If not, set it as mapped
		void* data;																						//Create the pointer and assign it the mapped memory address
		vkMapMemory(lux::core::g::compute.LD, lux::core::c::CBuffers[buffer].memory, getCellOffset(&lux::core::g::compute.PD, vCell), getCellSize(vCell), 0, &data);
		return data;																					//Return the pointer
	}
}