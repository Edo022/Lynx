

#include "LuxEngine/Core/Engine.h"




//TODO use unifom buffer with small cells
//Creates a memory buffer in a compute device and saves it in the lux::Array "computeBuffers"
//*   vSize          | the size in bytes of the buffer
//*   vBufferClass   | the class of the buffer
//*   vCpuAccessible | whether or not the CPU can access the buffer. Non accessible memory is faster but cannot be mapped
//*       Trying to map a non accessible cell will result in an access violation error
//			TODO fix
//*   Returns        | the index of the created buffer. -1 if an error occurs
LuxBuffer Engine::gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible) {
	LuxBuffer_t buffer(vSize, vBufferClass, vCpuAccessible);	//Create the buffer struct
	createBuffer(												//Create the vkBuffer as a storage buffer with transfer source and destination capabilities
		compute.LD, vSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		(vCpuAccessible) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&buffer.buffer, &buffer.memory
	);
	return CBuffers.add(buffer);								//Add it to the buffer array and return its index
}




//Creates a memory cell into a buffer. Allocates a new buffer if there are no compatible buffers with free cells
//*   vCellSize      | the size in bytes of the cell
//*   vCpuAccessible | whether the CPU can access the cell or not. Non accessible memory is faster but it cannot be mapped
//*   Returns        | the code of the cell. -1 if an error occurs
LuxCell Engine::gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible) {
	LuxBufferClass bufferClass;																		//Create a variable that stores the class of the buffer
	if(vCellSize <= LUX_BUFFER_CLASS_50) bufferClass = LUX_BUFFER_CLASS_50;							//Find the required buffer class. A cell must have a size smaller than or equal to the size of the class to belong to it
	else if(vCellSize <= LUX_BUFFER_CLASS_5K) bufferClass = LUX_BUFFER_CLASS_5K;
	else if(vCellSize <= LUX_BUFFER_CLASS_500K) bufferClass = LUX_BUFFER_CLASS_500K;
	else if(vCellSize <= LUX_BUFFER_CLASS_2MLN) bufferClass = LUX_BUFFER_CLASS_2MLN;
	else bufferClass = LUX_BUFFER_CLASS_LRG;

	if(bufferClass != LUX_BUFFER_CLASS_LRG) {														//If it's a static buffer
		LuxBuffer buffer = -1;																			//Initialize the buffer variable. It stores the index of the buffer where the cell will be created
		for(uint32 i = 0; i < CBuffers.size( ); ++i) {													//Find the buffer. For each of the preexistent buffers
			if(CBuffers.isValid(i) &&																		//It can be used
				CBuffers[i].cpuAccessible == vCpuAccessible &&												//It's of the same memory type,
				CBuffers[i].bufferClass == bufferClass &&													//If it's of the right class,
				CBuffers[i].cells.usedSize( ) < LUX_CNF_GPU_STATIC_BUFFER_SIZE / CBuffers[i].bufferClass) {	//And it has free cells,
				buffer = i;																					//Save its index
			}
		}
		if(buffer == (LuxBuffer)-1) buffer = gpuBufferCreate(LUX_CNF_GPU_STATIC_BUFFER_SIZE, bufferClass, vCpuAccessible);	//If no buffer was found, create a new one with the specified class and a size equal to the static buffer default size and save its index
		return createLuxCell(true, buffer, CBuffers[buffer].cells.add(1), bufferClass);										//If there is buffer of the same type, create a new cell in it and return its code
	}
	else return createLuxCell(false, gpuBufferCreate(vCellSize, LUX_BUFFER_CLASS_LRG, vCpuAccessible), 0, vCellSize);	//If it's a custom size buffer, create a new buffer and return its code
}




//TODO choose device
//Removes a memory cell from the cell list, destroys the vulkan object and frees its memory
//*   vCell   | the LuxCell to destroy
//*   returns | true if the operation succeeded, false if the cell is invalid
bool Engine::gpuCellDestroy(const LuxCell vCell) {
	LuxBuffer buffer = getBufferIndex(vCell);								//Get buffer index
	//TODO Use lux output console
	if(!CBuffers.isValid(buffer)) {
		Failure printf("Something went wrong .-.");
		Failure printf("you were trying to use an invalid index buffer %llu", buffer);
		return false;								//If the buffer index is not valid, return false
	}

	if(CBuffers.isValid(buffer)) {												//If it's not removed
		if(CBuffers[buffer].bufferClass == LUX_BUFFER_CLASS_LRG) {					//And the buffer is a custom size buffer
			destroyBuffer://TODO destroy the old if there is one (rm if n+1 fb)
			//TODO Use lux output console
			if(!CBuffers.isValid(buffer)) {
				Failure printf("Something went wrong .-.");
				Failure printf("you were trying to use an invalid index buffer %llu", buffer);
				return false;									//If the buffer index is not valid, return false
			}
			vkDestroyBuffer(graphics.LD, CBuffers[buffer].buffer, nullptr);				//destroy the GPU buffer structure
			vkFreeMemory(graphics.LD, CBuffers[buffer].memory, nullptr);				//Free the buffer's memory
			CBuffers.remove(buffer, false);												//Remove the buffer from the buffer array
			return true;
		}
		else {																		//If it's a fixed size buffer
			//TODO Use lux output console
			if(CBuffers[buffer].cells.remove(getCellIndex(vCell))) {
				Failure printf("Something went wrong .-.");
				Failure printf("you were trying to use an invalid cell index %llu", getCellIndex(vCell));
				return false;		//And the cell index is invalid, return false
			}
			if(CBuffers[buffer].cells.usedSize( ) == 0) goto destroyBuffer;				//If it's valid, remove the cell. If there are no cells left, destroy the buffer
		}
	}
	else return false;
}




//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
//Mapping an already mapped buffer will overwrite the old mapping
//*   buffer  | a pointer to a LuxBuffer_t object. It's the buffer that will be mapped
//*   returns | the void pointer that maps the buffer, nullptr if an error occurs
void* Engine::gpuCellMap(const LuxCell vCell) {
	LuxBuffer buffer = getBufferIndex(vCell);														//Get the buffer index
	//TODO Use lux output console
	if(!CBuffers.isValid(buffer)) {
		Failure printf("Something went wrong .-.");
		Failure printf("you were trying to use an invalid index buffer %llu", buffer);
		return nullptr;													//If the buffer index is not valid, return nullptr
	}

	if(CBuffers[buffer].isMapped) vkUnmapMemory(compute.LD, CBuffers[buffer].memory);				//If it's already mapped, unmap the shared memory
	else CBuffers[buffer].isMapped = true;															//If not, set it as mapped
	void* data;																						//Create the pointer and assign it the mapped memory address
	vkMapMemory(compute.LD, CBuffers[buffer].memory, getCellOffset(&compute.PD, vCell), getCellSize(vCell), 0, &data);
	return data;																					//Return the pointer
}
