

#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Devices.h"







namespace lux::core::c::buffers{
	Array<uint32> uniformSizes{ 50, 5000, 500000, 2000000, 0 };
	Array<uint32> storageSizes{ 50, 5000, 500000, 2000000, 0 };
	Map<LuxBuffer_t, uint32> CBuffers; //List of GPU buffers

	void init( ){
		for(int i = 0; i < (uint32)LuxBufferClass::LUX_BUFFER_CLASS_NUM; i++){
			uniformSizes[i] = scast<uint32>(multipleOf(uniformSizes[i], dvc::compute.PD.properties.limits.minStorageBufferOffsetAlignment));
			storageSizes[i] = scast<uint32>(multipleOf(storageSizes[i], dvc::compute.PD.properties.limits.minUniformBufferOffsetAlignment));
		}
	}








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
		TryVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer)) printError("Failed to create buffer");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{ };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, vProperties);

		//TODO check out of memory
		//TODO don't quit in VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS but return an error code
		switch(vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {	//IF out of device memory, use the host memory
				VkMemoryAllocateInfo allocInfo2{ };
				allocInfo2.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo2.allocationSize = memRequirements.size;
				allocInfo2.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr, pMemory)) {
					case VK_SUCCESS: break;
					case VK_ERROR_OUT_OF_HOST_MEMORY: //TODO add case. same as next out of host memory
					default: printError("Failed to allocate buffer memory");
				}
				break;
			}
			case VK_ERROR_OUT_OF_HOST_MEMORY:	//TODO If out of host memory
			case VK_ERROR_TOO_MANY_OBJECTS:		//TODO
			default: printError("Failed to allocate buffer memory");
		}

		TryVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0)) printError("Failed to bind buffer");
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
		VkCommandBuffer commandBuffer = g::cmd::beginSingleTimeCommands( );		//Start command buffer
		vkCmdCopyBuffer(commandBuffer, vSrcBuffer, vDstBuffer, 1, &copyRegion);	//Record the copy command
		g::cmd::endSingleTimeCommands(commandBuffer);							//End command buffer
	}




	//TODO fix all the fucked up comments



	//TODO use unifom buffer with small cells
	//Creates a memory buffer in a compute device and saves it in the lux::Array "computeBuffers"
	//*   vSize          | the size in bytes of the buffer
	//*   vBufferClass   | the class of the buffer
	//*   vCpuAccessible | whether or not the CPU can access the buffer. Non accessible memory is faster but cannot be mapped
	//*       Trying to map a non accessible cell will result in an access violation error
	//*   vReadOnly      | if the buffer is read-only to the shader
	//*       Read only buffers are created as uniform buffers and are much faster than normal storage buffers
	//*       Too large read only buffers are creates as storage buffer
	//*   Returns        | the index of the created buffer. -1 if an error occurs
	LuxBuffer gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible, const bool vReadOnly) {
		//Create the buffer struct
		LuxBuffer_t buffer{
			.size{ vSize },
			.bufferClass{ vBufferClass },
			.cpuAccessible{ vCpuAccessible },
			.readOnly{ vReadOnly }
		};

		createBuffer(												//Create the vkBuffer as a storage buffer with transfer source and destination capabilities
			dvc::compute.LD, vSize,
			//Choose type. Uniform buffer if the data fits in the max uniform buffer size and it's a read only buffer, Storage buffer otherwise
			//TODO wrong maxUniformBufferRange. It's UINT_MAX, for some reason
			((vReadOnly && (core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize)) ?
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			//Choose memory type. Shared memory if it needs to be accessed by the CPU, local GPU memory if not
			(vCpuAccessible) ?
			(VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&buffer.buffer, &buffer.memory
		);

		return buffers::CBuffers.add(buffer);						//Add it to the buffer array and return its index
	}




	//Creates a memory cell into a buffer. Allocates a new buffer if there are no compatible buffers with free cells
	//*   vCellSize      | the size in bytes of the cell
	//*   vCpuAccessible | whether the CPU can access the cell or not. Non accessible memory is faster but it cannot be mapped
	//*   Returns        | the code of the cell. -1 if an error occurs
	LuxCell gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible, const bool vReadOnly) {
		LuxBufferClass bClass;														//Create a variable that stores the class of the buffer
		if(vCellSize <= LUX_BUFFER_CLASS_50)		bClass = LUX_BUFFER_CLASS_50; 		//Find the required buffer class. A cell must have a size smaller than or equal to the size of theUniformto belong to i] : [bufferStorageClassesSizest
		else if(vCellSize <= LUX_BUFFER_CLASS_5K)	bClass = LUX_BUFFER_CLASS_5K;
		else if(vCellSize <= LUX_BUFFER_CLASS_500K) bClass = LUX_BUFFER_CLASS_500K;
		else if(vCellSize <= LUX_BUFFER_CLASS_2MLN) bClass = LUX_BUFFER_CLASS_2MLN;
		else bClass = LUX_BUFFER_CLASS_LRG;

		if(bClass != LUX_BUFFER_CLASS_LRG) {										//If it's a static buffer
			LuxBuffer buffer = -1;														//Initialize the buffer variable. It stores the index of the buffer where the cell will be created
			for(uint32 i = 0; i < buffers::CBuffers.size( ); ++i) {						//Find the buffer. For each of the preexistent buffers
				if(buffers::CBuffers.isValid(i) &&											//If it can be used,
					buffers::CBuffers[i].readOnly == vReadOnly &&							//It has the same type,
					buffers::CBuffers[i].cpuAccessible == vCpuAccessible &&					//same memory type,
					buffers::CBuffers[i].bufferClass == bClass &&							//same class,    //And it has free cells,
					buffers::CBuffers[i].cells.usedSize( ) < LUX_CNF_GPU_STATIC_BUFFER_SIZE / buffers::CBuffers[i].bufferClass) {
					buffer = i;																	//Save its index
				}
			}
			//If there are no buffers of the same type, create a new one with the specified class and properties and save its index
			//If there is a buffer of the same type, create a new cell in it and return its code
			if(buffer == (LuxBuffer)-1) buffer = gpuBufferCreate(LUX_CNF_GPU_STATIC_BUFFER_SIZE, bClass, vCpuAccessible, vReadOnly);
			return createLuxCell(true, buffer, buffers::CBuffers[buffer].cells.add(1), (vReadOnly) ? storageSizes[bClass] : uniformSizes[bClass]);
		}
		//If it's a custom size buffer, create a new buffer and return its code
		else return createLuxCell(false, gpuBufferCreate(vCellSize, LUX_BUFFER_CLASS_LRG, vCpuAccessible, vReadOnly), 0, vCellSize);
	}




	//TODO choose device
	//Removes a memory cell from the cell list, destroys the vulkan object and frees its memory
	//*   vCell   | the LuxCell to destroy
	//*   returns | true if the operation succeeded, false if the cell is invalid
	bool gpuCellDestroy(const LuxCell vCell) {
		LuxBuffer buffer = getBufferIndex(vCell);		//Get buffer index
		//TODO fix buffer validity check
		//TODO Use lux output console
		if(!buffers::CBuffers.isValid(buffer)) {										//If the buffer index is invalid
			printError("Something went wrong .-.  you were trying to use an invalid index buffer (see error code)", buffer, false);
			return false;																	//Print an error and return false
		}
		else {																			//If it's valid
			if(buffers::CBuffers[buffer].bufferClass == LUX_BUFFER_CLASS_LRG) {				//And the buffer is a custom size buffer
				destroyBuffer://TODO destroy the old if there is one (rm if n+1 fb)
				vkDestroyBuffer(dvc::graphics.LD, buffers::CBuffers[buffer].buffer, nullptr);	//destroy the GPU buffer structure
				vkFreeMemory(dvc::graphics.LD, buffers::CBuffers[buffer].memory, nullptr);		//Free the buffer's memory
				buffers::CBuffers.remove(buffer, false);										//Remove the buffer from the buffer array
				return true;
			}
			else {																			//If it's a fixed size buffer
				if(buffers::CBuffers[buffer].cells.remove(getCellIndex(vCell))) {				//Try to remove the cell from the buffer's cells
					printError("Something went wrong .-.  you were trying to use an invalid cell buffer (see error code)", buffer, false);
					return false;																//If the cell index is invalid, print an error and return false
				}
				else{																			//If it's valid, remove the cell
					if(buffers::CBuffers[buffer].cells.usedSize( ) == 0) goto destroyBuffer;		//If there are no cells left, destroy the buffer
					return true;																	//Otherwise do nothing and return true
				}
			}
		}
	}




	//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
	//Mapping an already mapped buffer will overwrite the old mapping
	//*   buffer  | a pointer to a LuxBuffer_t object. It's the buffer that will be mapped
	//*   returns | the void pointer that maps the buffer, nullptr if an error occurs
	void* gpuCellMap(const LuxCell vCell) {
		LuxBuffer buffer = getBufferIndex(vCell);		//Get the buffer index
		//TODO Use lux output console
		if(!buffers::CBuffers.isValid(buffer)) {		//If the buffer index is not valid
			Failure core::printError("Something went wrong .-.  you were trying to use an invalid index buffer (see error core)", false, buffer);
			return nullptr;									//print an error and return nullptr
		}

		if(buffers::CBuffers[buffer].isMapped) vkUnmapMemory(dvc::compute.LD, buffers::CBuffers[buffer].memory);//If it's already mapped, unmap the shared memory
		else buffers::CBuffers[buffer].isMapped = true;			//If not, set it as mapped
		void* data;												//Create the pointer and assign it the mapped memory address
		vkMapMemory(dvc::compute.LD, buffers::CBuffers[buffer].memory, getCellOffset(&dvc::compute.PD, vCell), getCellSize(vCell), 0, &data);
		return data;											//Return the pointer
	}
}