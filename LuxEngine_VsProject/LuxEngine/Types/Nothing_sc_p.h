#pragma once
#include "LuxEngine/macros.h"




#define lux_sc_v 0x94FFD489B48994FF
#define lux_sc_F luxDebug(lux_sc_F_f( ));							// void foo( ) { lux_sc_F ... }
#define lux_sc_N luxDebug2(lux_sc_initialized{ lux_sc_N_f( ) },)	// s( lux::Nothing ) : lux_sc_N { ... }
#define lux_sc_C luxDebug2(lux_sc_initialized{ lux_sc_C_f( ) },)	// s( ) : lux_sc_C { ... }


#define lux_sc_generate_debug_structure_body						\
	luxDebug(uint64 lux_sc_initialized = lux_sc_v);					\
																	\
	luxDebug(void lux_sc_F_f( ) const {	/*Operators and Functions*/	\
		if(lux_sc_initialized != lux_sc_v) printError("Structures must be initialized before using their pointers or non assignment/init/constructor functions", true, -1);	\
	})																\
	luxDebug(uint64 lux_sc_N_f( ){	/*lux::Nothing constructor*/	\
		if(lux_sc_initialized != lux_sc_v) printError("Structures created with a lux::Nothing constructor must be initialized before their default initialization", true, -1);	\
		return lux_sc_v;											\
	})																\
	luxDebug(uint64 lux_sc_C_f( ){	/*Other constructors*/			\
		return lux_sc_v;											\
	})
