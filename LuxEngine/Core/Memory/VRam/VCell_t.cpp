#include "LuxEngine/Core/Memory/VRam/VCell_t.hpp"




namespace lux::vram{
    Type_t2 types[((uint32)__pvt::VCellClassIndex::NUM << 2) | 0b11];
    RaArrayC<Cell_t2> cells;
    std::mutex cells_m;
}