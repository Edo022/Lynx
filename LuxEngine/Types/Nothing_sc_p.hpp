#pragma once
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"



#define lux_sc_v 0x94FFD489B48994FF
#define checkInit luxDebug(lux_sc_F_f( ))	// void foo( ) lux_sc_F, var{ 0 } {...}
#define lux_sc_generate_nothing_constructor(structName)	inline structName(const lux::Nothing) : luxDebug2(lux_sc_dummy{ lux_sc_N_f( ) },)
//TODO check if it has been initialized




#define lux_sc_generate_debug_structure_body luxDebug(						\
	uint64 lux_sc_initialized = lux_sc_v;									\
	lux_sc_generate_debug_structure_body_func_only;							\
)


#define lux_sc_generate_debug_structure_body_func_only	luxDebug(			\
	char lux_sc_dummy = 'L';												\
	inline void lux_sc_F_f( ) const {	/*Operators and Functions*/			\
		if(this->lux_sc_initialized != lux_sc_v) printError("Structures must be initialized before using their pointers or non assignment/init/constructor functions", true, -1);		\
	}																		\
	inline char lux_sc_N_f( ){			/*lux::Nothing constructor*/		\
		if(this->lux_sc_initialized != lux_sc_v) printError("Structures created with a lux::Nothing constructor must be initialized before their default initialization", true, -1);	\
		return 0;															\
	}																		\
)






#define isInit(var)				luxDebug(luxCheckParam(var.lux_sc_initialized != lux_sc_v, var, "A structure must be initialized before being used"));
#define checkCount				luxCheckCond(count( ) == 0, "This function cannot be called on containers with size 0")
#define checkSize				luxCheckCond(size( ) == 0,  "This function cannot be called on pointers pointing to a 0-byte memory allocation")
#define constructExec(fun, ...) luxDebug(lux_sc_dummy{fun(__VA_ARGS__) ? lux_sc_dummy : lux_sc_dummy},)
