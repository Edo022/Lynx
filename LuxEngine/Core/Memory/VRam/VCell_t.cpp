#include "LuxEngine/Core/Memory/VRam/VCell_t.hpp"


//Cell used in null allocations to skip some nullptr checks
// thread_local lux::ram::Cell_t lux::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };


namespace lux::vram{
    Type_t2 types[((uint32)__pvt::VCellClassIndex::NUM << 2) | 0b11];
    RaArrayC<Cell_t2> cells;
    std::mutex cells_m;
}