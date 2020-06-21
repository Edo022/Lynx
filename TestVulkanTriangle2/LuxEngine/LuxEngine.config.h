#pragma once


//This defines if the program is compiled in debug or in release mode
#ifndef NDEBUG 
#define LUX_DEBUG
#else
#undef LUX_DEBUG
#endif



//This is the precision of the pre-computed values of the goniometric functions
//Default: 100000. 800KB per function. 7200KB total
#define FUNC_PRECISION 100000