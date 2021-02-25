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


		struct Cell_t {
			uint16 typeIndex;		//INDEX of the buffer type. -1 for custom size cells		//2
			uint16 owners;			//Number of lux::ram::ptr instances that owns the cell		//4
			uint32 cellIndex;		//Index of the cell in the cells array						//8
			uint32 localIndex;		//Index of the cell in the type allocations					//12
			uint32 cellSize;		//Size of the cell in bytes									//16
			void*  address;			//Address of the cell										//24
			//char padding[8]																	//32
			// char padding[28 luxDebug(-16)];														//60
			//uint32 next;																		//64
			luxDebug(Alloc<Dummy>* lastOwner;)
			luxDebug(Alloc<Dummy>* firstOwner;)
		};

		struct alignas(64) Type_t {
			CellClass cellClass;	//Class of the cells
			void** memory;			//Addresses of the buffers
			RaArrayC<bool> cells;	//TODO use optimized uint64 array
			uint32 cellsPerBuff;	//Number of cells in each buffer
			std::mutex m; //FIXME REMOVE
		};

		extern Type_t types[];		//Allocated buffers
		extern RaArrayC<Cell_t> cells;
		extern thread_local Cell_t dummyCell;

		extern std::mutex cells_m;
		extern uint32 allocated;	//TODO remove
	}
}