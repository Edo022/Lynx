#pragma once
#define LUX_H_MEMORY_T
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/Memory/LuxMap_NMP_S.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include <cstring>






#define LuxMemOffset 32
namespace lux{
	//Bytes allocated to each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets //FIXME align on 32 bytes
	enum class CellClass : uint32 {
		CLASS_A = 32U,					//32 bytes. The minimum size of a cell
		CLASS_B = CLASS_A * 16,			//16x CLASS_A. 512 B per cell (~0.5KB)
		CLASS_C = CLASS_A * 32,			//32x CLASS_A. 1024 B per cell (~1KB)
		CLASS_D = CLASS_A * 64,			//64x CLASS_A. 2048 B per cell (~2KB)
		CLASS_Q = CLASS_D * 64,			//64x CLASS_D. 131072 B per cell (~130KB)
		CLASS_L = CLASS_Q * 64,			//64x CLASS_Q. 8388608 B per cell (~8.4MB)
		CLASS_0 = 0,					//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,				//Chooses a class large enough to contain the cell

		AT_LEAST_CLASS_A = 1 + CLASS_A,	//Like AUTO, but the class is guaranteed to be at least CLASS_A
		AT_LEAST_CLASS_B = 1 + CLASS_B,	//Like AUTO, but the class is guaranteed to be at least CLASS_B
		AT_LEAST_CLASS_C = 1 + CLASS_C,	//Like AUTO, but the class is guaranteed to be at least CLASS_C
		AT_LEAST_CLASS_D = 1 + CLASS_D,	//Like AUTO, but the class is guaranteed to be at least CLASS_D
		AT_LEAST_CLASS_Q = 1 + CLASS_Q,	//Like AUTO, but the class is guaranteed to be at least CLASS_Q
		AT_LEAST_CLASS_L = 1 + CLASS_L,	//Like AUTO, but the class is guaranteed to be at least CLASS_L
	};


	namespace __pvt{
		//Indices corresponding to lux::CellClass values
		//Engine iternal use
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
		//Size of each buffer. 50331648 B (~50MB)
		static const uint32 bufferSize = (uint32)CellClass::CLASS_L * 6;


		//Returns the CellClassIndex value of a CellClass
		static constexpr inline uint32 classIndexFromEnum(const CellClass vClass){
			switch(vClass){
				#define _case(n) case CellClass::CLASS_##n: return (uint32)CellClassIndex::INDEX_##n;
				_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0)
				default: luxPrintError("Invalid cell class %d", (uint32)vClass) return (uint32)-1;
			}
		}
		//Returns the CellClass value of a CellClassIndex
		static constexpr CellClass classes[] = { CellClass::CLASS_A, CellClass::CLASS_B, CellClass::CLASS_C, CellClass::CLASS_D, CellClass::CLASS_Q, CellClass::CLASS_L };
		static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex){
			return classes[(uint64)vIndex];
		}
	}








	namespace ram{
		//! If you modify those variables change the declarations in Cell_t.hpp too
		struct MemBufferType;
		extern MemBufferType* 	buffers;	//Allocated buffers
		extern uint32 			allocated;	//TODO remove

		struct Cell_t;
		struct MemBuffer {
			void* 						memory = nullptr;	//Address of the buffer
			Map_NMP_S<Cell_t, uint32> 	cells;				//Cells in the buffer
			uint32 						bufferIndex;		//Index of the buffer in the MemBufferType buffers array
		};
		struct MemBufferType {
			CellClass 					cellClass;			//Class of the cells
			Map_NMP_S<MemBuffer, uint32> buffers;			//Buffers containing the cells
		};
		struct Cell_t {
			uint32 			owners;				//Number of lux::ram::ptr instances that point to an address of the cell
			uint64 			cellSize;			//Size of the cell in bytes
			void* 			address;			//Address of the cell. The same as you would get with malloc
			MemBufferType* 	bufferType;			//Type of buffer allocation
			MemBuffer* 		buffer;				//Index of the buffer where the cell is allocated
			uint32 			cellIndex;			//Index of the cell in the buffer
			void free();
		};
		static inline uint32 getCellOffset(const Cell_t* pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }
	}
}