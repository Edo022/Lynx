#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include <intrin.h>





//TODO check for alllocation number limit
//TODO use multiple devices
namespace lux::rem{
	//Bytes to allocate for each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets
	enum CellClass : uint32 {
		CELL_CLASS_A = 32,						//32 bytes. The minimum size of a cell
		CELL_CLASS_B = CELL_CLASS_A * 16,	//16x CELL_CLASS_A. 512 B per cell (~0.5KB)
		CELL_CLASS_C = CELL_CLASS_A * 32,	//32x CELL_CLASS_A. 1024 B per cell (~1KB)
		CELL_CLASS_D = CELL_CLASS_A * 64,	//64x CELL_CLASS_A. 2048 B per cell (~2KB)
		CELL_CLASS_Q = CELL_CLASS_D * 64,	//64x CELL_CLASS_D. 131072 B per cell (~130KB)
		CELL_CLASS_L = CELL_CLASS_Q * 64,	//64x CELL_CLASS_Q. 8388608 B per cell (~8.4MB)
		CELL_CLASS_0 = 0,						//Dedicated buffer for cells larger than CELL_CLASS_L
		CELL_CLASS_AUTO = (uint32)-1,			//Choose a class large enough to contain the cell
	};
	enum CellClassIndex : uint32 {
		CELL_CLASS_INDEX_A = 0b000,
		CELL_CLASS_INDEX_B = 0b001,
		CELL_CLASS_INDEX_C = 0b010,
		CELL_CLASS_INDEX_D = 0b011,
		CELL_CLASS_INDEX_Q = 0b100,
		CELL_CLASS_INDEX_L = 0b101,
		CELL_CLASS_INDEX_0 = 0b110,
		CELL_CLASS_NUM 							//Number of LUX_CELL_CLASS values
	};
	enum AllocType : uint32 {
		ALLOC_TYPE_DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		ALLOC_TYPE_DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		ALLOC_TYPE_SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		ALLOC_TYPE_SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		ALLOC_TYPE_NUM							//Number of LUX_ALLOC_TYPE values
	};
	const uint32 bufferSize = CELL_CLASS_L * 6;	//Size of each buffer. 50331648 B (~50MB)



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
		//void* address;			//Address of the cell. The same as you would get with malloc
		MemBufferType* bufferType;	//Type of buffer allocation
		MemBuffer* buffer;			//Index of the buffer where the cell is allocated
		uint32 cellIndex;			//Index of the cell in the buffer
	};
	typedef Cell_t* Cell;



	static constexpr inline bool isUniform(const AllocType vAllocType) { return vAllocType & 0b1; }
	static constexpr inline bool isShared(const AllocType vAllocType) { return vAllocType >> 1; }




	extern uint32 maxAlloc;						//The maximum number of allocated buffers. Depends on the gpu properties
	extern Array<MemBufferType> buffers;		//VRAM allocated buffers
	void init( );




	Cell alloc(const uint64 vSize, const CellClass vCellClass, const AllocType vAllocType);
	void* map(Cell pCell);
	static inline void unmap(Cell pCell){ vkUnmapMemory(core::dvc::compute.LD, pCell->buffer->memory); }
	void free(Cell pCell);

	//Generates the index of a buffer from the cell class and allocation type
	// 1 0 1 | 0 1
	// class | type
	static inline uint32 getCellOffset(const Cell pCell){ return pCell->bufferType->cellClass * pCell->cellIndex; }
}
