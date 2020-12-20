#pragma once
#define LUX_H_CELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Core/Memory/CellMng_t.hpp"


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
		struct Buffer_t;
		struct Type_t;

		//! If you modify those variables change the declarations in Ram.hpp too
		extern Type_t types[];		//Allocated buffers
		extern uint32 allocated;	//TODO remove



		struct Cell_t {
			uint32 		owners;		//Number of lux::ram::ptr instances that point to an address of the cell
			uint32 		cellIndex;	//Index of the cell
			uint32 		cellSize;	//Size of the cell in bytes //FIXME add option to choose to cache variables like this or not (this value can be retrieved from the buffer type, but it's slower)
			// Type_t* 	type;		//Type of buffer allocation
			uint32 		type;		//INDEX of the type
			// Buffer_t* 	buffer;		//Index of the buffer where the cell is allocated
			void* 		address;	//Address of the cell. The same as you would get with malloc
			luxDebug(lux::__pvt::CellState state;)
			void free();
		};
		// struct Buffer_t { //TODO rename as Buffer_t
		// 	void* memory = nullptr;	//Address of the buffer
		// 	uint32 bufferIndex;		//Index of the buffer in the Type_t buffers array
		// 	Cell_t* cells;			//Cells in the buffer //FIXME CHOOSE NUMBER OF CELLS PER BUFFER OR SOMETHING IDK
		// };
		struct Type_t { //TODO rename as Type_t
			const CellClass cellClass;	//Class of the cells
			// Buffer_t* buffers;		//Buffers containing the cells
			//!
			void** memory;	//Addresses of the buffers
			Cell_t* cells;				//Cells
			uint32* cellsll;
			uint32 head, tail;
			uint32 cellsPerBuff; //cellsPerBuff = cellsNum / buffsNum
		};
		// static inline uint32 getCellOffset(const Cell_t* pCell){ return (uint32)pCell->type->cellClass * pCell->cellIndex; }
	}
}