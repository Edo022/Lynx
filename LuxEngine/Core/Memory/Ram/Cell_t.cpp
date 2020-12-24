#include "LuxEngine/Core/Memory/Ram/Cell_t.hpp"
#include "LuxEngine/Core/Memory/Ram/Ram.hpp"


//Cell used in null allocations to skip some nullptr checks
lux::ram::Cell_t lux::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };





//TODO destroy buffers from asyncrhonous garbage collector
//TODO or destroy them only when there is not enough memory
//TODO this does not free the cell. it just makes the element invalid
//TODO check if the cell is accidentally being re-allocated or not
