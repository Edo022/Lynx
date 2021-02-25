#include "LuxEngine/Core/Memory/Gpu/VCell_t.hpp"
// #include "LuxEngine/Core/Memory/Gpu/Ram.hpp"


//Cell used in null allocations to skip some nullptr checks
// thread_local lux::ram::Cell_t lux::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };


namespace lux::rem{
    Type_t2 types[];		//Allocated buffers
    RaArrayC<Cell_t2> cells;
    std::mutex cells_m;
}