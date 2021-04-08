#include "Lynx/Core/Memory/Ram/Cell_t.hpp"
#include "Lynx/Core/Memory/Ram/Ram.hpp"


//Cell used in null allocations to skip some nullptr checks
thread_local lnx::ram::Cell_t lnx::ram::dummyCell = { .owners = 0, .cellSize = 0, .address = nullptr };
