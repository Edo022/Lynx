#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include <intrin.h>





namespace lux{
	//Bytes to allocate for each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets
	enum class CellClass : uint32 {
		CLASS_A = 32,				//32 bytes. The minimum size of a cell
		CLASS_B = CLASS_A * 16,		//16x CLASS_A. 512 B per cell (~0.5KB)
		CLASS_C = CLASS_A * 32,		//32x CLASS_A. 1024 B per cell (~1KB)
		CLASS_D = CLASS_A * 64,		//64x CLASS_A. 2048 B per cell (~2KB)
		CLASS_Q = CLASS_D * 64,		//64x CLASS_D. 131072 B per cell (~130KB)
		CLASS_L = CLASS_Q * 64,		//64x CLASS_Q. 8388608 B per cell (~8.4MB)
		CLASS_0 = 0,				//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,			//Choose a class large enough to contain the cell
	};
	enum class CellClassIndex : uint32 {
		INDEX_A = 0b000,
		INDEX_B = 0b001,
		INDEX_C = 0b010,
		INDEX_D = 0b011,
		INDEX_Q = 0b100,
		INDEX_L = 0b101,
		INDEX_0 = 0b110,
		NUM 						//Number of LUX_CELL_CLASS values
	};
	enum class AllocType : uint32 {
		DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		NUM							//Number of LUX_ALLOC_TYPE values
	};
	static const uint32 bufferSize = (uint32)CellClass::CLASS_L * 6;	//Size of each buffer. 50331648 B (~50MB)




	namespace ram{
		class Cell_t;
		struct MemBuffer {
			void* memory;					//Address of the buffer
			Map<Cell_t, uint32> cells;		//Cells in the buffer
		};
		struct MemBufferType {
			CellClass cellClass;			//Class of the cells
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
			void* address;				//Address of the cell. The same as you would get with malloc
			MemBufferType* bufferType;	//Type of buffer allocation
			MemBuffer* buffer;			//Index of the buffer where the cell is allocated
			uint32 cellIndex;			//Index of the cell in the buffer
		};
		typedef Cell_t* Cell;



		//static constexpr inline bool isUniform(const AllocType vAllocType) { return vAllocType & 0b1; }
		//static constexpr inline bool isShared(const AllocType vAllocType) { return vAllocType >> 1; }




		//extern uint32 maxAlloc;						//The maximum number of allocated buffers. Depends on the gpu properties
		//extern Array<MemBufferType> buffers;		//VRAM allocated buffers
		//void init( );




		//Cell alloc(const uint64 vSize, const CellClass vCellClass, const AllocType vAllocType);
		//void* map(Cell pCell);
		//static inline void unmap(Cell pCell){ vkUnmapMemory(core::dvc::compute.LD, pCell->buffer->memory); }
		//void free(Cell pCell);

		////Generates the index of a buffer from the cell class and allocation type
		//// 1 0 1 | 0 1
		//// class | type
		//static inline uint32 getCellOffset(const Cell pCell){ return pCell->bufferType->cellClass * pCell->cellIndex; }
	}
}
