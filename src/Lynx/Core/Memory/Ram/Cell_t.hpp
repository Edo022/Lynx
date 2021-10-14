#pragma once
#include "Lynx/Core/Memory/Ram/Classes.hpp"
#include "Lynx/Types/Containers/RaArrayC.hpp"
#include "Lynx/Types/Dummy.hpp"
#include "Lynx/Core/Init.hpp"
#include <mutex>








namespace lnx::ram{
	template<class tType> struct ptr;
	//! If you modify those variables change the declarations in Ram.hpp and Ram.cpp too


	struct Cell_t {
		uint16 typeIndex;		//INDEX of the buffer type. -1 for custom size cells		//2
		uint16 owners;			//Number of lnx::ram::ptr instances that owns the cell		//2
		uint32 cellIndex;		//Index of the cell in the cells array						//4
		uint32 localIndex;		//Index of the cell in the type allocations					//4
		uint32 cellSize;		//Size of the cell in bytes									//4
		void*  address;			//Address of the cell										//8
		_dbg(ptr<Dummy>* lastOwner;)													    //[8]
		_dbg(ptr<Dummy>* firstOwner;)													    //[8]
	};

	struct alignas(64) Type_t {
		CellClass cellClass;	//Class of the cells
		void** memory;			//Addresses of the buffers
		RaArrayC<bool> cells;	//TODO use optimized uint64 array
		uint32 cellsPerBuff;	//Number of cells in each buffer
		std::mutex m; 			//FIXME REMOVE
	};

	_lnx_init_var_array_dec((Type_t), types);		//Allocated buffers
	_lnx_init_var_dec((RaArrayC<Cell_t, uint32>), cells);
	_lnx_init_var_dec((std::mutex), cells_m);

	static thread_local Cell_t dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr }; //FIXME this variable should be external thread_local

	_lnx_init_fun_dec(LNX_H_CELL_T);
}