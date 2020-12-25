#pragma once
#define LUX_H_CELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
// #include "LuxEngine/Core/Memory/CellMng_t.hpp"
#include "LuxEngine/Types/Containers/RaArrayC.hpp"


// #ifdef _WIN64
// #	include <windows.h>
// 	namespace lux::ram{
// 		//System RAM in bytes
// 		const uint64 systemMemory = [](){
// 			MEMORYSTATUSEX status;
// 			status.dwLength = sizeof(status);
// 			GlobalMemoryStatusEx(&status);
// 			return status.ullTotalPhys;
// 		}();
// 	}
// #elif defined(__linux__)
// #	include <unistd.h>
// 	namespace lux::ram{
// 		//System RAM in bytes
// 		const uint64 systemMemory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
// 	}
// #endif






// #define LuxMemOffset 32
#define LuxMemOffset 512
#define LuxIncSize ((uint32)CellClass::CLASS_L / 2)
namespace lux{
	namespace ram{
		template<class type> struct Alloc;
		struct Type_t;
		struct Cell_t;


		//! If you modify those variables change the declarations in Ram.hpp too
		extern Type_t types[];		//Allocated buffers
		extern uint32 allocated;	//TODO remove
		extern RaArrayC<Cell_t> cells;


		struct Cell_t {
			uint16 typeIndex;		//INDEX of the buffer type
			uint16 owners;			//Number of lux::ram::ptr instances that owns the cell
			uint32 cellIndex;		//Index of the cell in the cells array
			uint32 localIndex;		//Index of the cell in the type allocations
			uint32 cellSize;		//Size of the cell in bytes
			void*  address;			//Address of the cell
			luxDebug(Alloc<uint32>* lastOwner;)
			luxDebug(Alloc<uint32>* firstOwner;)
		};

		struct Type_t {
			CellClass cellClass;	//Class of the cells
			void** memory;			//Addresses of the buffers
			RaArrayC<bool> cells;	//TODO use optimized uint64 array
			uint32 cellsPerBuff;	//Number of cells in each buffer
		};

		extern Cell_t dummyCell;
	}
}