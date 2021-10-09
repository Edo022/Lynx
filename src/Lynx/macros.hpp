#pragma once
////#define LNX_H_MACROS


#ifdef __INTELLISENSE__
	#include ".engine/conf.hpp"
	//!The file is generated in the .engine directory of the user application when they change the build configuration
	//!This include is only used to allow Intellisense to correctly parse the code
#endif




#undef max
#undef min
//Functions
template<class ta, class tb>						static inline constexpr auto max(const ta a, const tb b) { return (a > b) ? a : b; } //Returns the minimum value
template<class ta, class tb>						static inline constexpr auto min(const ta a, const tb b) { return (a < b) ? a : b; } //Returns the maximum value
template<class ta, class tb, class tc, class ...tn> static inline constexpr auto max(const ta a, const tb b, const tc c, const tn... n) { return max(a, b, c, n...); }
template<class ta, class tb, class tc, class ...tn> static inline constexpr auto min(const ta a, const tb b, const tc c, const tn... n) { return min(a, b, c, n...); }




#ifdef LNX_DBG
#	define _dbg(...) __VA_ARGS__	//Executes a line of code only if in debug   mode
#	define _rls(...)				//Executes a line of code only if in release mode
#else
#	define _dbg(...)				//Executes a line of code only if in debug   mode
#	define _rls(...) __VA_ARGS__	//Executes a line of code only if in release mode
#endif




//Im lazy UwU
#define scast static_cast
#define rcast reinterpret_cast
#define noop  ((void)0)
#define alwaysInline __attribute__((__always_inline__,warning("function marked alwaysInline cannot be inlined"))) inline //TODO write that the function definition must be available even when compiling the .o
#define neverInline  __attribute__((__noinline__))
#define used  __attribute__((__used__))

#define alignVar(n) __attribute__((aligned(n )))
#define alignCache  __attribute__((aligned(64)))


#ifdef LNX_DBG
	#undef alwaysInline
	#define alwaysInline inline
#endif


//Time
#include <chrono>
#include <thread>
#define sleep(ms)					std::this_thread::sleep_for(std::chrono::milliseconds(ms))




#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpmf-conversions"
	/**
	 * @brief Returns true if the object's class redefines a virtual member function of a base class
	 *     e.g. if(doesRedefine(derivedInstance, &Obj_bb::func)) //...do something
	 * @param object An instance of the derived class
	 * @param vVMFP The virtual member function pointer of the base class
	 */
	#define doesRedefine(vObj, vVMFP) ((void*)((vObj).*(vVMFP)) != (void*)(vVMFP))
	#pragma GCC diagnostic pop
#else
	static neverInline __attribute__((optimize("O0"), error("\"doesRedefine\" macro is only available in g++"))) bool doesRedefine(auto vObj, auto vVMFP){ return true; }
#endif




//Returns the relative path to the engine files
neverInline const char* getEnginePath();

#ifdef __INTELLISENSE__
	//FIXME USE EXTERN CONST CHAR INSTEAD OF MACRO AND FUNCTION
	//Path to the engine files. Generated during user app compilation
	//This macro may only be used by the user application
	//DO NOT use this macro in the engine code. Call getEnginePath() instead.
	#define enginePath "<generated>"
#endif









// //Calculates the square root of a float number
// static float __s(float n) {
// 	static int32_t i = 0x5F3759DF - (*(int32_t*)&n >> 1);
// 	static float n2 = *(float*)&i;
// 	return scast<float>(1 / (n2 * (1.5 - (n * 0.5 * n2 * n2))));
// }

// //Calculates the result of b to the power of e. Way faster than math.h pow function
// static int64_t pow___(int64_t b, int64_t e) {
// 	int64_t r = 1;
// 	while (e > 0) {
// 		if (e & 1) r *= b % 1000000007;
// 		b *= b % 1000000007;
// 		e >>= 1;
// 	}
// 	return r;
// }