#include "LuxEngine/Memory/Memory_t.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Core/Core.h"

void lux::ram::Cell_t::freeCell( ){
	lux::ram::free(this);
}


luxDebug(void __lp_printWarning(const char* text){ printWarning(text); })
