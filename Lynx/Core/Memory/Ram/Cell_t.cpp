#include "Lynx/Core/Memory/Ram/Cell_t.hpp"
// #include "Lynx/Core/Memory/Ram/Ram.hpp"
#include "Lynx/Core/Init.hpp"
#include "Lynx/System/SystemInfo.hpp"



//Cell used in null allocations to skip some nullptr checks
// thread_local lnx::ram::Cell_t lnx::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };
//FIXME this variable is defined in the header as a static thread_local. It should be external thread_local

namespace lnx::ram{
	_lnx_init_var_array_def(Type_t, types, (uint32)__pvt::CellClassIndex::eNum){}
	_lnx_init_var_set_def(RaArrayC<Cell_t>, cells){}
	_lnx_init_var_set_def(std::mutex, cells_m){}





	LnxAutoInit(LNX_H_CELL_T) {
		using namespace lnx::ram::__pvt;

		//Initialize buffer types. Allocate enough cells and buffers to use the whole RAM
		for(uint32 i = 0; i < (uint32)CellClassIndex::eNum; ++i) {
			printf("RAM USED\n"); fflush(stdout); //TODO REMOVE DEBUG OUTPUT
			uint32 buffsNum = sys::ram.size / buffSize;						//Get max number of cells that can fit in the system memory
			uint32 cellsPerBuff = buffSize / (uint32)classEnumFromIndex(i);	//Get number of cells in each buffer
			new(&types[i]) Type_t{
				.cellClass = classEnumFromIndex(i),								//Set class index
				.memory =  (void** )calloc(sizeof(void* ),  buffsNum),			//Allocate the max number of buffers. Initialize them with nullptr
				.cellsPerBuff = cellsPerBuff
			};
			types[i].cells.init(cellsPerBuff * buffsNum);
		}
		cells.init(sys::ram.size / (uint64)CellClass::eA);
	}
}