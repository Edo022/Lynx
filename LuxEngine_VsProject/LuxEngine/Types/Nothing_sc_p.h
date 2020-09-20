#pragma once
#include "LuxEngine/macros.h"
#include "LuxEngine/Core/ConsoleOutput.h"



#define lux_sc_v 0x94FFD489B48994FF
#define lux_sc_F luxDebug(lux_sc_F_f( ))	// void foo( ) lux_sc_F, var{ 0 } {...}
#define lux_sc_generate_nothing_constructor(structName)				inline structName(const lux::Nothing) : luxDebug2(lux_sc_initialized{ lux_sc_N_f( ) },)



#define lux_sc_generate_debug_structure_body								\
	luxDebug(uint64 lux_sc_initialized = lux_sc_v);							\
																			\
	luxDebug(inline void lux_sc_F_f( ) const {	/*Operators and Functions*/	\
		if(lux_sc_initialized != lux_sc_v) printError("Structures must be initialized before using their pointers or non assignment/init/constructor functions", true, -1);	\
	})																		\
	luxDebug(inline uint64 lux_sc_N_f( ){	/*lux::Nothing constructor*/	\
		if(lux_sc_initialized != lux_sc_v) printError("Structures created with a lux::Nothing constructor must be initialized before their default initialization", true, -1);	\
		return lux_sc_v;													\
	})																		\
