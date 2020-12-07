#pragma once
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"



namespace lux::__pvt{
	//Use checkInit; or checkInitParam(param); to
	constexpr uint64 init_val = 0x94FFD489B48994FF;
	//Private variables used in initialization checks. Those members cannot be inherited
	#define genInitCheck	luxDebug(uint64 __pvt_init_val = lux::__pvt::init_val; char __pvt_dummy = 'L')


	//Prints an error if the count of a lux::ContainerBase is 0
	#define checkCount				luxCheckCond(count( ) == 0, "This function cannot be called on containers with size 0")
	//Prints an error if the size of a lux::ContainerBase is 0
	#define checkSize				luxCheckCond(size( ) == 0,  "This function cannot be called on pointers pointing to a 0-bytes memory block")
	//Checks if a function parameter is initialized
	#define checkInit 				luxCheckCond(this->__pvt_init_val != lux::__pvt::init_val, "This function cannot be called on uninitialized structures");
	//Checks if a function parameter is initialized
	#define checkInitParam(var)		luxDebug(luxCheckParam(var.__pvt_init_val != lux::__pvt::init_val, var, "The structure is uninitialized"));
	//Executes a function in a costructor. Debug mode only
	#define constructExec(fun, ...) luxDebug(__pvt_dummy{ fun(__VA_ARGS__) ? 'L' : 'K' },)
}