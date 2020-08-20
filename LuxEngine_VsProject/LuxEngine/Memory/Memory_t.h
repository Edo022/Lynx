#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"








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
	#define _case(n) case CellClass::CLASS_##n: return (uint32)CellClassIndex::INDEX_##n;
	static constexpr inline uint32 classIndexFromEnum(const CellClass vClass){ switch(vClass){ _case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0) default: return (uint32)-1; } };
	#define _case2(n) case CellClassIndex::INDEX_##n: return CellClass::CLASS_##n;
	static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex){ switch(vIndex){ _case2(A) _case2(B) _case2(C) _case2(D) _case2(Q) _case2(L) _case2(0) default: return (CellClass)-1; } };




	namespace ram{
		struct Cell_t;
		struct MemBuffer {
			void* memory;					//Address of the buffer
			Map<Cell_t, uint32> cells;		//Cells in the buffer
		};
		struct MemBufferType {
			CellClass cellClass;			//Class of the cells
			Map<MemBuffer, uint32> buffers;	//Buffers containing the cells
		};
		struct Cell_t {
			uint64 cellSize;				//Size of the cell in bytes
			void* address;					//Address of the cell. The same as you would get with malloc
			MemBufferType* bufferType;		//Type of buffer allocation
			MemBuffer* buffer;				//Index of the buffer where the cell is allocated
			uint32 cellIndex;				//Index of the cell in the buffer
		};
		typedef Cell_t* Cell;
		static inline uint32 getCellOffset(const Cell pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }
	}
}