#pragma once
#pragma warning(disable : 4005) //Macro referinition
#include "LuxEngine_config.hpp"


//TODO remove this header
#undef max
#undef min
//Functions
template<class ta, class tb>						static inline constexpr auto max(const ta a, const tb b)							{ return (a > b) ? a : b; }				//Returns the minimum value
template<class ta, class tb>						static inline constexpr auto min(const ta a, const tb b)							{ return (a < b) ? a : b; }				//Returns the maximum value
template<class ta, class tb, class tc>				static inline constexpr auto max(const ta a, const tb b, const tc c)				{ return max(max(a, b), c); }			//Returns the minimum value
template<class ta, class tb, class tc>				static inline constexpr auto min(const ta a, const tb b, const tc c)				{ return min(min(a, b), c); }			//Returns the maximum value
template<class ta, class tb, class tc, class td>	static inline constexpr auto max(const ta a, const tb b, const tc c, const td d)	{ return max(max(max(a, b), c), d); }	//Returns the minimum value
template<class ta, class tb, class tc, class td>	static inline constexpr auto min(const ta a, const tb b, const tc c, const td d)	{ return min(min(min(a, b), c), d); }	//Returns the maximum value
template<class ta, class tb>						static inline constexpr void swapVar(ta a, tb b)									{ a ^= b ^= a ^= b; }					//Swaps the contents of 2 basic type variables


//Console output
#include "deps/TermColor.hpp"
#define Normal				std::cout<<termcolor::white		<< '\n';
#define NormalNoNl			std::cout<<termcolor::white;
#define Success				std::cout<<termcolor::green		<< '\n';
#define SuccessNoNl			std::cout<<termcolor::green;
#define Main				std::cout<<termcolor::magenta	<< '\n';
#define Failure				std::cout<<termcolor::red		<< '\n';
#define Warning				std::cout<<termcolor::yellow	<< '\n';
#define MainSeparator		Normal printf("\n\n#---------------------------------------------#\n\n");




//Debug
#define luxDebug(...)				__VA_ARGS__
#define luxRelease(...)

#ifndef LUX_DEBUG
#define luxDebug(...)
#define luxRelease(...)				__VA_ARGS__
#endif


//Im lazy UwU
#define scast						static_cast
#define rcast						reinterpret_cast


//Time
#include <chrono>
#include <thread>
typedef std::chrono::system_clock::time_point LuxTime;
#define luxStartChrono()			std::chrono::system_clock::now()
#define luxStopChrono(start)		((scast<std::chrono::duration<double>>(luxStartChrono() - (start))).count())
#define sleep(ms)					std::this_thread::sleep_for(std::chrono::milliseconds(ms))




#pragma warning(default : 4005) //Macro referinition
















//TODO
//Calculates the square root of a float number. Sometimes this function is slightly slower than math.h sqrt function
static float __s(float n) {
	static int32_t i = 0x5F3759DF - (*(int32_t*)&n >> 1);
	static float n2 = *(float*)&i;
	return scast<float>(1 / (n2 * (1.5 - (n * 0.5 * n2 * n2))));
}

//Calculates the result of b to the power of e. Way faster than math.h pow function
static int64_t pow___(int64_t b, int64_t e) {
	int64_t r = 1;
	while (e > 0) {
		if (e & 1) r *= b % 1000000007;
		b *= b % 1000000007;
		e >>= 1;
	}
	return r;
}

