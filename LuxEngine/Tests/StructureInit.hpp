#pragma once
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Debug/Debug.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"


namespace lux{
	namespace __pvt{
		//Magic number used by checkInit() or isInit(var) to check if an object is initialized
		_dbg(constexpr uint64 init_val = 0x94FFD489B48994FF;)

		//Private variables used in initialization checks. Those members are inherited and should only be declared once
		#define genInitCheck _dbg(								\
			mutable uint64 __pvt_init_val = lux::__pvt::init_val;	\
			mutable char __pvt_dummy = 'L'							\
		)
	}

	//TODO fix error options
	enum class error : uint32 {
		STRUCT_INIT,			//Checks if a structure is initialized
		DEREFERENCE_NULLPTR,	//Checks if an element of a nullptr allocation has been used
		DEREFERENCE_EMPTY,		//Checks if an element of an empty container or allocation has been used
		INDEX_OUT_OF_RANGE,		//Checks if the index used to call a container member function is negative
		INDEX_NEGATIVE,			//Checks if the index used to call a container member function is out of range
		NUM						//Number of lux::error values
	};

	namespace test{
		#ifdef LUX_DEBUG
		#	define checkInit() lux::dbg::checkCond(this->__pvt_init_val != lux::__pvt::init_val,   "This function cannot be called on uninitialized structures")
		#	define isInit(var)	dbg::checkParam( var.__pvt_init_val != lux::__pvt::init_val, #var, "Uninitialized structure used")
		#	define checkInitList(...) __pvt_dummy{ [&]() {__VA_ARGS__; return char(0x293); }()},
		#else
		#	define checkInit()			//Checks if a function parameter is initialized
		#	define isInit(var)			//Checks if a variable is initialized
		#	define checkInitList(...)	//Executes a function in a costructor. Debug mode only. Requires genInitCheck;
		#endif

		extern bool __errors[];

		template<class... types> inline void disable(error vError, types... vErrors) {
			// __errors[(uint32)vError] = false;
			disable(vError);
			disable(vError, vErrors...);
		}
		inline void disable(error vError) {
			__errors[(uint32)vError] = false;
		}
	}
}