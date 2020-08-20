#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include <intrin.h>





//TODO check for alllocation number limit
//TODO use multiple devices
namespace lux::rem{
	class Cell_t;
	struct MemBuffer {
		VkBuffer buffer;				//Vulkan buffer object
		VkDeviceMemory memory;			//Vulkan buffer memory
		Map<Cell_t, uint32> cells;		//Cells in the buffer
	};
	struct MemBufferType {
		CellClass cellClass;			//Class of the cells
		AllocType allocType;			//Buffer allocation type
		Map<MemBuffer, uint32> buffers;	//Buffers containing the cells
	};
	// |--------------------------------- VRAM -------------------------------|
	// |------- Buffer0 ------||------- Buffer1 ------||------- Buffer2 ------|
	// |-cell-||-cell-||-cell-|
	//A video memory cell
	//A cell is a fixed-size partition of memory inside an allocated GPU buffer
	//Allocate a cell with the lux::rem::alloc function
	struct Cell_t {
		uint64 cellSize;			//Size of the cell in bytes
		MemBufferType* bufferType;	//Type of buffer allocation
		MemBuffer* buffer;			//Index of the buffer where the cell is allocated
		uint32 cellIndex;			//Index of the cell in the buffer

		void* map();
		inline void unmap(){ vkUnmapMemory(core::dvc::compute.LD, buffer->memory); }
	};
	typedef Cell_t* Cell;
	Cell alloc(const uint64 vSize, const CellClass vCellClass, const AllocType vAllocType);
	void free(Cell pCell);



	extern uint32 maxAlloc;						//The maximum number of allocated buffers. Depends on the gpu properties
	extern Array<MemBufferType> buffers;		//VRAM allocated buffers
	void init( );





	//Generates the index of a buffer from the cell class and allocation type
	// 1 0 1 | 0 1
	// class | type
	static inline uint32 getCellOffset(const Cell pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }
}
