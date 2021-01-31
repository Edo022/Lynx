#pragma once
#include "LuxEngine_config.hpp"



//TODO remove this header
#undef max
#undef min
//Functions
template<class ta, class tb>						static inline constexpr void swapVar(ta& a, tb& b)		 { a ^= b ^= a ^= b; }       //Swaps the contents of 2 basic type variables
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



#ifdef __INTELLISENSE__
    #define LUX_DEBUG
#endif
#ifdef LUX_DEBUG
#	define luxDebug(...) __VA_ARGS__                                //Executes a line of code only if in debug   mode
#	define luxRelease(...)                                          //Executes a line of code only if in release mode
#   define alwaysInline inline                                      //Indicates that a function must be inlined. Functions are never inlined, in debug mode
#   define forceInline __attribute__ ((__always_inline__)) inline   //Inlines a function even if in debug mode
#else
#	define luxDebug(...)
#	define luxRelease(...) __VA_ARGS__
#   define alwaysInline __attribute__ ((__always_inline__)) inline
#   define forceInline alwaysInline
#endif


//Im lazy UwU
#define scast   static_cast
#define rcast   reinterpret_cast
#define noop    ((void)0)
#define neverInline  __attribute__ ((__noinline__))


//Time
#include <chrono>
#include <thread>
typedef std::chrono::system_clock::time_point LuxTime;
#define luxStartChrono()			std::chrono::system_clock::now()
#define luxStopChrono(start)		((scast<std::chrono::duration<double>>(luxStartChrono() - (start))).count())
#define sleep(ms)					std::this_thread::sleep_for(std::chrono::milliseconds(ms))




//Returns the relative path to the engine files
neverInline const char* getEnginePath();















// //TODO
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



// #define pure __attribute__ ((pure))
// #define hot  __attribute__ ((hot))