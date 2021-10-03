#include "Lynx/Core/Memory/Ram/Cell_t.hpp"
// #include "Lynx/Core/Memory/Ram/Ram.hpp"


//Cell used in null allocations to skip some nullptr checks
// thread_local lnx::ram::Cell_t lnx::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };
//FIXME this variable is defined in the header as a static thread_local. It should be external thread_local

namespace lnx::ram{
	__init_var_array_def(Type_t, types, (uint32)__pvt::CellClassIndex::eNum){}
	__init_var_set_def(RaArrayC<Cell_t>, cells){}
	__init_var_set_def(std::mutex, cells_m){}
}