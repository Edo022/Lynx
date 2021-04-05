#pragma once
#define LUX_H_MACROS
#include "LuxEngine_config.hpp"
#include ".engine/conf.hpp"
//! ^ This file is generated in the .engine directory of the user application when they change the build configuration



#undef max
#undef min
//Functions
template<class ta, class tb>						static inline constexpr auto max(const ta a, const tb b) { return (a > b) ? a : b; } //Returns the minimum value
template<class ta, class tb>						static inline constexpr auto min(const ta a, const tb b) { return (a < b) ? a : b; } //Returns the maximum value
template<class ta, class tb, class tc, class ...tn> static inline constexpr auto max(const ta a, const tb b, const tc c, const tn... n) { return max(a, b, c, n...); }
template<class ta, class tb, class tc, class ...tn> static inline constexpr auto min(const ta a, const tb b, const tc c, const tn... n) { return min(a, b, c, n...); }




//Console output
#include "LuxEngine/Core/__tmp__OutputColor.hpp"
#define Normal				std::cout << __tmp_output_color::white << '\n';
#define NormalNoNl			std::cout << __tmp_output_color::white;
#define Success				std::cout << __tmp_output_color::green << '\n';
#define SuccessNoNl			std::cout << __tmp_output_color::green;
#define Main				std::cout << __tmp_output_color::magenta << '\n';
#define Failure				std::cout << __tmp_output_color::red << '\n';
#define Warning				std::cout << __tmp_output_color::yellow	<< '\n';
#define MainSeparator		Normal printf("\n\n#---------------------------------------------#\n\n");




#ifdef LUX_DEBUG
#	define _dbg(...) __VA_ARGS__   //Executes a line of code only if in debug   mode
#	define _rls(...)               //Executes a line of code only if in release mode
#else
#	define _dbg(...)
#	define _rls(...) __VA_ARGS__
#endif




//Im lazy UwU
#define scast   static_cast
#define rcast   reinterpret_cast
#define noop    ((void)0)
#define alwaysInline __attribute__((__always_inline__,warning("function marked alwaysInline cannot be inlined"))) inline
#define neverInline  __attribute__ ((__noinline__))

#define alignVar(n) __attribute__((aligned(n )))
#define alignCache  __attribute__((aligned(64)))


#ifdef LUX_DEBUG
    #undef alwaysInline
    #define alwaysInline
#endif


//Time
#include <chrono>
#include <thread>
typedef std::chrono::system_clock::time_point LuxTime;
#define luxStartChrono()			std::chrono::system_clock::now()
#define luxStopChrono(start)		((scast<std::chrono::duration<double>>(luxStartChrono() - (start))).count())
#define sleep(ms)					std::this_thread::sleep_for(std::chrono::milliseconds(ms))




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