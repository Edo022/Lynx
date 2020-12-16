#pragma once
#define LUX_H_CELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/__nmp_RaArray.hpp"








#define LuxMemOffset 32
namespace lux{
	namespace ram{
		struct MemBuffer;
		struct MemBufferType;

		//! If you modify those variables change the declarations in Ram.hpp too
		extern MemBufferType* 	buffers;	//Allocated buffers
		extern uint32 			allocated;	//TODO remove


		struct Cell_t {
			uint32 			owners;				//Number of lux::ram::ptr instances that point to an address of the cell
			uint64 			cellSize;			//Size of the cell in bytes
			void* 			address;			//Address of the cell. The same as you would get with malloc
			MemBufferType* 	bufferType;			//Type of buffer allocation
			MemBuffer* 		buffer;				//Index of the buffer where the cell is allocated
			uint32 			cellIndex;			//Index of the cell in the buffer
			void free();
		};
		struct MemBuffer {
			void* 						memory = nullptr;	//Address of the buffer
			// Map_NMP_S<Cell_t, uint32> 	cells;				//Cells in the buffer
			__nmp_RaArray<Cell_t, uint32, 32> 	cells;				//Cells in the buffer //FIXME CHOOSE NUMBER OF CELLS PER BUFFER OR SOMETHING IDK
			uint32 						bufferIndex;		//Index of the buffer in the MemBufferType buffers array
		};
		struct MemBufferType {
			CellClass 					cellClass;			//Class of the cells
			// Map_NMP_S<MemBuffer, uint32> buffers;			//Buffers containing the cells
			__nmp_RaArray<MemBuffer, uint32, 32> buffers;			//Buffers containing the cells
		};
		static inline uint32 getCellOffset(const Cell_t* pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }
	}
}