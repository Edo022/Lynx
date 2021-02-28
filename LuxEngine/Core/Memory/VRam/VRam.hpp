#pragma once
#define LUX_H_VMEMORY
#include "LuxEngine/Types/Integers/Integers.hpp"




//TODO check for alllocation number limit
namespace lux::vram{
	// typedef Cell_t* Cell;
	// // static inline uint32 getCellOffset(const Cell pCell) { return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }
	//BUG USE CELL OFFSET NOT TYPE OFFSET WTF
	// template<class type> static inline uint32 getCellOffset(const Alloc_b<type> pCell) { return pCell.cell->cellIndex * sizeof(type); }

	// Cell allocBck(const uint64 vSize, const CellClass vCellClass, const lux::AllocType vAllocType);
	// void free(Cell pCell);



	// extern uint32 maxAlloc;				//The maximum number of allocated buffers. Depends on the gpu properties
	// extern MemBufferType* buffers;		//VRAM allocated buffers
}
