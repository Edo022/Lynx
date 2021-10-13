#include "Lynx/Core/Memory/VRam/VCell_t.hpp"
#include "Lynx/Core/Init.hpp"
#include "Lynx/System/SystemInfo.hpp"




namespace lnx::vram{
    _lnx_init_var_redirect_array_def((Type_t2), types, ((uint32)_pvt::VCellClassIndex::eNum << 2) | 0b11, lnx::vram){}
    _lnx_init_var_redirect_value_def((RaArrayC<Cell_t2, uint32>), cells,   lnx::vram){}
    _lnx_init_var_redirect_value_def((std::mutex),                cells_m, lnx::vram){}


    _lnx_init_fun_def(LNX_H_VCELL_T, lnx::vram) {
		using namespace vram::_pvt;

		//Initialize buffer types. Allocate enough cells and buffers to use the whole RAM
		for(uint32 k = 0; k < 2; ++k) { 									//Loop location
			for(uint32 j = 0; j < 2; ++j) { 									//Loop buffer type
				for(uint32 i = 0; i < (uint32)VCellClassIndex::eNum; ++i) {			//Loop cell size
					uint32 buffsNum = sys::g_vram().size / buffSize;						//Get max number of cells that can fit in the system memory
					uint32 typeIndex = (i << 2) | (j << 1) | k;							//Calculate buffer type index
					uint32 cellsPerBuff = buffSize / (uint32)classEnumFromIndex(i);		//Get number of cells in each buffer
					new(&vram::g_types()[typeIndex]) vram::Type_t2{							//Create new type struct
						.cellClass = classEnumFromIndex(i),									//Set class index
						.memory =  (vram::Cell_t2_csc* )calloc(sizeof(vram::Cell_t2_csc),  buffsNum),//Allocate the max number of buffers. Initialize them with nullptr
						.cellsPerBuff = cellsPerBuff										//Set the maximum number of cells per buffer
					};
					vram::g_types()[typeIndex].cells.init(cellsPerBuff * buffsNum);
				}
			}
		}
		vram::g_cells().init(sys::g_vram().size / (uint64)vram::VCellClass::eA);
	}

}