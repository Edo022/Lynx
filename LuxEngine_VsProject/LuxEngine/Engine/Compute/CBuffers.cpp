

#include "LuxEngine/Engine/Engine.h"





	//Creates a memory buffer in a compute device and saves it in the lux::Array "computeBuffers"
	//*   vSize: the size in bytes of the buffer
	//*   vBufferClass: the class of the buffer
	//*   vCpuAccessible: whether the CPU can access the buffer or not. Non accessible memory is faster but cannot be mapped
	//*       Trying to map a non accessible cell will result in an access violation error
	//*   Returns the index of the buffer in the array. -1 if an error occurs
	LuxBuffer Engine::gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible) {
		LuxBuffer_t buffer;					//Create the buffer struct
		buffer.size = vSize;				//Set its size and create the vkBuffer as an host visible storage buffer with transfer source capabilities
		createBuffer(
			compute.LD, buffer.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, //TODO unifom buffer with small cells
			(vCpuAccessible) ? VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&buffer.buffer, &buffer.memory
		);
		buffer.bufferClass = vBufferClass;		//Set its class
		buffer.cpuAccessible = vCpuAccessible;	//Set CPU accessibility
		return CBuffers.add(buffer);			//Add it to the buffer array and return its index
	}
	//TODO ctrl+u ctrl+shift+u




	//Creates a memory cell into a buffer. Allocates a new buffer if there are no compatible buffers with free cells
	//*   vCellSize: the size in bytes of the cell
	//*   vCpuAccessible: whether the CPU can access the cell or not. Non accessible memory is faster but it cannot be mapped
	//*   Returns the code of the cell. -1 if an error occurs
	LuxCell Engine::gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible) {
		LuxBufferClass bufferClass;																	//Create a variable that stores the class of the buffer
		if(vCellSize <= LUX_BUFFER_CLASS_50) bufferClass = LUX_BUFFER_CLASS_50;						//Find the required buffer class. A cell must have a size smaller than or equal to the size of the class to belong to it
		else if(vCellSize <= LUX_BUFFER_CLASS_5K) bufferClass = LUX_BUFFER_CLASS_5K;
		else if(vCellSize <= LUX_BUFFER_CLASS_500K) bufferClass = LUX_BUFFER_CLASS_500K;
		else if(vCellSize <= LUX_BUFFER_CLASS_2MLN) bufferClass = LUX_BUFFER_CLASS_2MLN;
		else bufferClass = LUX_BUFFER_CLASS_LRG;

		if(bufferClass != LUX_BUFFER_CLASS_LRG) {													//If it's a static buffer
			LuxBuffer buffer = -1;																		//Initialize the buffer variable. It stores the index of the buffer where the cell will be created
			for(uint32 i = 0; i < CBuffers.size( ); ++i) {												//Find the buffer. For each of the preexistent buffers
				if(CBuffers.isValid(i) &&																	//It can be used
					CBuffers[i].cpuAccessible == vCpuAccessible &&											//It's of the same memory type,
					CBuffers[i].bufferClass == bufferClass &&												//If it's of the right class,
					CBuffers[i].cells.usedSize( ) < GPU_STATIC_BUFFER_SIZE / CBuffers[i].bufferClass) {		//And it has free cells,
					buffer = i;																					//Save its index
				}
			}
			if(buffer == (LuxBuffer)-1) buffer = gpuBufferCreate(GPU_STATIC_BUFFER_SIZE, bufferClass, vCpuAccessible);			//If no buffer was found, create a new one with the specified class and a size equal to the static buffer default size and save its index
			return __lp_cellCode(true, buffer, CBuffers[buffer].cells.add(1), bufferClass);										//Create a new cell in the buffer and return its code
		}
		else return __lp_cellCode(false, gpuBufferCreate(vCellSize, LUX_BUFFER_CLASS_LRG, vCpuAccessible), 0, vCellSize);	//If it's a custom size buffer, create a new buffer and return its code
	}




	//Removes a memory cell from the cell list, destroys the vulkan object and frees its memory
	//*   vCell: the LuxCell to destroy
	//*   returns true if the operation succeeded, false if the cell is invalid
	bool Engine::gpuCellDestroy(const LuxCell vCell) {
		LuxBuffer buffer = __lp_buffer_from_cc(vCell);
		if(CBuffers.isValid(buffer)) {
			//TODO choose device
			if(CBuffers[buffer].bufferClass == LUX_BUFFER_CLASS_LRG) {
				destroyBuffer:
				vkDestroyBuffer(graphics.LD, CBuffers[buffer].buffer, null);
				vkFreeMemory(graphics.LD, CBuffers[buffer].memory, null);
				CBuffers.remove(buffer, false);
				return true;
			}
			else {
				if(!(CBuffers[buffer].cells.remove(__lp_cellIndex_from_cc(vCell)))) return false;;
				if(CBuffers[buffer].cells.usedSize( ) == 0) goto destroyBuffer;
				return true;
			}
		}
		else return false;
	}




	//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
	//Mapping an already mapped buffer will overwrite the old mapping
	//*   buffer: a pointer to a LuxBuffer_t object. It's the buffer that will be mapped
	//*   returns the void pointer that maps the buffer
	void* Engine::gpuCellMap(const LuxCell vCell) {
		LuxBuffer buffer = __lp_buffer_from_cc(vCell);
		if(CBuffers[buffer].isMapped) vkUnmapMemory(compute.LD, CBuffers[buffer].memory);
		else CBuffers[buffer].isMapped = true;

		uint32 offset = __lp_cellOffset_from_cc(&compute.PD, vCell);
		void* data;
		vkMapMemory(compute.LD, CBuffers[buffer].memory, offset, __lp_cellSize_from_cc(vCell), 0, &data);
		return data;
	}
