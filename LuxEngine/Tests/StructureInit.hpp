#pragma once
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"



namespace lux{
	namespace __pvt{
		//Use checkInit(); or isInit(param); to
		constexpr uint64 init_val = 0x94FFD489B48994FF;
		//Private variables used in initialization checks. Those members cannot be inherited
		#define genInitCheck	luxDebug(uint64 __pvt_init_val = lux::__pvt::init_val; char __pvt_dummy = 'L')
	}

	namespace test{
		//checkInit() -- Checks if a function parameter is initialized
		//isInit(var) -- Checks if a variable is initialized
		//constructExec(func, ...) -- Executes a function in a costructor. Debug mode only. Requires genInitCheck;
		#define checkInit() luxCheckCond(this->__pvt_init_val != lux::__pvt::init_val,      "This function cannot be called on uninitialized structures")
		#define isInit(var)	luxCheckParam( var.__pvt_init_val != lux::__pvt::init_val, var, "Uninitialized structure used")
		#define constructExec(...) luxDebug(__pvt_dummy{ [](){__VA_ARGS__; return char(0x293)}()},)
	}
}