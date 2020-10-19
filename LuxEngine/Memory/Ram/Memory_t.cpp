#include "LuxEngine/Memory/Memory_t.h"
#include "LuxEngine/Memory/Ram/Memory.h"


void lux::ram::Cell_t::freeCell( ){
	lux::ram::free(this);
}

