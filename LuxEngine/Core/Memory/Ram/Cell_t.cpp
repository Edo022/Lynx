#include "LuxEngine/Core/Memory/Ram/Cell_t.hpp"
#include "LuxEngine/Core/Memory/Ram/Ram.hpp"


//Cell used in null allocations to skip some nullptr checks
thread_local lux::ram::Cell_t lux::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };





//TODO destroy buffers from asyncrhonous garbage collector
//TODO or destroy them only when there is not enough memory
