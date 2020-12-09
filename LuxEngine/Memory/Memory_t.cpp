#include "LuxEngine/Memory/Memory_t.hpp"
#include "LuxEngine/Memory/Ram/Memory.hpp"


void lux::ram::Cell_t::freeCell( ){
	lux::ram::__pvt::free(this);
}
