#pragma once
#define LUX_H_MEMORY_T
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/Memory/LuxMap_NMP_S.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include "LuxEngine/Core/Memory/Shared.hpp"
#include <cstring>






#define LuxMemOffset 32
namespace lux{
	namespace ram{
		//! If you modify those variables change the declarations in Ram.hpp too
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