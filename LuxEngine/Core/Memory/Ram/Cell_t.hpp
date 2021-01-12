#pragma once
#define LUX_H_CELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/RaArrayC.hpp"
#include "LuxEngine/Types/Dummy.hpp"








#define LuxMemOffset 512
#define LuxIncSize ((uint32)CellClass::CLASS_L / 2)
namespace lux{
	namespace ram{
		template<class type> struct Alloc;
		//! If you modify those variables change the declarations in Ram.hpp too
		struct Type_t;
		extern Type_t types[];		//Allocated buffers
		extern uint32 allocated;	//TODO remove


		struct Cell_t {
			uint16 typeIndex;		//INDEX of the buffer type. -1 for custom size cells
			uint16 owners;			//Number of lux::ram::ptr instances that owns the cell
			uint32 cellIndex;		//Index of the cell in the cells array
			uint32 localIndex;		//Index of the cell in the type allocations
			uint32 cellSize;		//Size of the cell in bytes
			void*  address;			//Address of the cell
			luxDebug(Alloc<Dummy>* lastOwner;)
			luxDebug(Alloc<Dummy>* firstOwner;)
		};

		struct Type_t {
			CellClass cellClass;	//Class of the cells
			void** memory;			//Addresses of the buffers
			RaArrayC<bool> cells;	//TODO use optimized uint64 array
			uint32 cellsPerBuff;	//Number of cells in each buffer
		};

		extern RaArrayC<Cell_t> cells;
		extern Cell_t dummyCell;
	}
}