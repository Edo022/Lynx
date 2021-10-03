#include "Lynx/Core/Memory/VRam/VCell_t.hpp"




namespace lnx::vram{
    __init_var_array_def(Type_t2, types, ((uint32)__pvt::VCellClassIndex::eNum << 2) | 0b11){}
    __init_var_set_def(RaArrayC<Cell_t2>, cells){}
    __init_var_set_def(std::mutex, cells_m){}
}