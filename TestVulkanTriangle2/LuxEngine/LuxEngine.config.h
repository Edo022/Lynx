#pragma once


//This defines if the program is compiled in debug or in release mode
#ifndef NDEBUG 
#define LUX_DEBUG
#else
#undef LUX_DEBUG
#endif



//This is the precision of the precomputed values of the goniometric functions
//0 means the engine will not use precomputed values and fast goniometric functions will be unavailable
//Default: 100000. 800KB per function. 7200KB total
#define FUNC_PRECISION 100000

