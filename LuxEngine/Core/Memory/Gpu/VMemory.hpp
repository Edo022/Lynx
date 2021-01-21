#pragma once
#define LUX_H_VMEMORY
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Render/CBuffers.hpp"

#include "LuxEngine/Core/Memory/Gpu/VCell_t.hpp" //FIXME replace with VPointer




//TODO check for alllocation number limit
namespace lux::rem{
	typedef Cell_t* Cell;
	static inline uint32 getCellOffset(const Cell pCell) { return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }

	Cell allocBck(const uint64 vSize, const CellClass vCellClass, const lux::AllocType vAllocType);
	void free(Cell pCell);



	extern uint32 maxAlloc;				//The maximum number of allocated buffers. Depends on the gpu properties
	extern MemBufferType* buffers;		//VRAM allocated buffers
}
