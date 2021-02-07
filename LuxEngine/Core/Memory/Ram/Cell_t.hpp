#pragma once
#define LUX_H_CELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/RaArrayC.hpp"
#include "LuxEngine/Types/Dummy.hpp"
#include <mutex>








#define LuxMemOffset 512
#define LuxIncSize ((uint32)CellClass::CLASS_L / 2)
namespace lux{
	namespace ram{
		template<class type> struct Alloc;
		//! If you modify those variables change the declarations in Ram.hpp and Ram.cpp too
		struct Type_t;
		extern Type_t types[];		//Allocated buffers
		extern std::mutex cells_m;
		extern uint32 allocated;	//TODO remove


		struct alignas(64) Cell_t {
			uint16 typeIndex;		//INDEX of the buffer type. -1 for custom size cells
			uint16 owners;			//Number of lux::ram::ptr instances that owns the cell
			uint32 cellIndex;		//Index of the cell in the cells array
			uint32 localIndex;		//Index of the cell in the type allocations
			uint32 cellSize;		//Size of the cell in bytes
			void*  address;			//Address of the cell
			luxDebug(Alloc<Dummy>* lastOwner;)
			luxDebug(Alloc<Dummy>* firstOwner;)
		};

		struct alignas(64) Type_t {
			CellClass cellClass;	//Class of the cells
			void** memory;			//Addresses of the buffers
			RaArrayC<bool> cells;	//TODO use optimized uint64 array
			uint32 cellsPerBuff;	//Number of cells in each buffer
			std::mutex m;
		};

		extern RaArrayC<Cell_t> cells;
		extern Cell_t dummyCell;
	}
}