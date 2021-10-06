#pragma once
////#define LNX_H_STRUCTURE_INIT
#include "Lynx/macros.hpp"
#include "Lynx/Core/Init.hpp"
#include "Lynx/Debug/Debug.hpp"
#include "Lynx/Types/Integers/Integers.hpp"




namespace lnx{
	namespace __pvt{
		//Magic number used by checkInit() or isInit(var) to check if an object is initialized
		_dbg(constexpr uint64 init_val = 0x94FFD489B48994FF;)

		//Private variables used in initialization checks. Those members are inherited and should only be declared once
		#ifdef LNX_DBG
			#define genInitCheck 										\
				mutable uint64 __pvt_init_val = lnx::__pvt::init_val;	\
				mutable char __pvt_dummy = 'L'
		#else
			#define genInitCheck
		#endif
	}


	//TODO fix error options
	enum class error : uint32 {
		eStructInit,			//Checks if a structure is initialized
		eDereferenceNullptr,	//Checks if an element of a nullptr allocation has been used
		eDereferenceEmpty,		//Checks if an element of an empty container or allocation has been used
		eIndexOutOfRange,		//Checks if the index used to call a container member function is negative
		eIndexNegative,			//Checks if the index used to call a container member function is out of range
		eNum					//Number of lnx::error values
	};




	namespace test{
		#ifdef LNX_DBG
		#	define checkInit() lnx::dbg::checkCond(this->__pvt_init_val != lnx::__pvt::init_val, "This function cannot be called on uninitialized structures")
		#	define isInit(var)      dbg::checkParam( var.__pvt_init_val != lnx::__pvt::init_val, #var, "Uninitialized structure used")
		#	define checkInitList(...) __pvt_dummy{ [&]() { __VA_ARGS__; return char(0x293); }() },
		#else
		#	define checkInit()			//Checks if a function parameter is initialized
		#	define isInit(var)			//Checks if a variable is initialized
		#	define checkInitList(...)	//Executes a function in a costructor. Debug mode only. Requires genInitCheck;
		#endif

		_lnx_init_var_array_dec(bool, __errors);


		/**
		 * @brief //TODO
		 */
		template<class... types> inline void disable(error vError, types... vErrors) {
			// __errors[(uint32)vError] = false;
			disable(vError);
			disable(vError, vErrors...);
		}

		/**
		 * @brief //TODO
		 */
		inline void disable(error vError) {
			g___errors()[(uint32)vError] = false;
		}
	}
}