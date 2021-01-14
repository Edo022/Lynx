#pragma once
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"


namespace lux{
	namespace __pvt{
		//Use checkInit(); or isInit(param); to
		constexpr uint64 init_val = 0x94FFD489B48994FF;
		//Private variables used in initialization checks. Those members cannot be inherited
		#define genInitCheck	luxDebug(uint64 __pvt_init_val = lux::__pvt::init_val; char __pvt_dummy = 'L')
	}

	enum class error : uint32 {
		STRUCT_INIT,			//Checks if a structure is initialized
		DEREFERENCE_NULLPTR,	//Checks if an element of a nullptr allocation has been used
		DEREFERENCE_EMPTY,		//Checks if an element of an empty container or allocation has been used
		INDEX_OUT_OF_RANGE,		//Checks if the index used to call a container member function is negative
		INDEX_NEGATIVE,			//Checks if the index used to call a container member function is out of range
		NUM						//Number of lux::error values
	};

	namespace test{
		//checkInit() -- Checks if a function parameter is initialized
		//isInit(var) -- Checks if a variable is initialized
		//checkInitList(func, ...) -- Executes a function in a costructor. Debug mode only. Requires genInitCheck;
		#define checkInit() lux::dbg::checkCond(this->__pvt_init_val != lux::__pvt::init_val,      "This function cannot be called on uninitialized structures")
		#define isInit(var)	dbg::checkParam( var.__pvt_init_val != lux::__pvt::init_val, #var, "Uninitialized structure used")
		#define checkInitList(...) luxDebug(__pvt_dummy{ [&](){__VA_ARGS__; return char(0x293); }()},)


		extern bool __errors[];

		template<class... types> inline void disable(error vError, types... vErrors){
			// __errors[(uint32)vError] = false;
			disable(vError);
			disable(vError, vErrors...);
		}
		inline void disable(error vError){
			__errors[(uint32)vError] = false;
		}
	}
}