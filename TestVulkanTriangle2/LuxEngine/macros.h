#pragma once
#pragma warning(disable : 4005) //Macro referinition
#pragma warning(disable : 4002) //Too many arguments in luxDebug and luxRelease
#include "LuxEngine.config.h"



//functions
template<class T> constexpr T __vectorcall max(T a, T b) { return (a > b) ? a : b; }					//Returns the minimum value
template<class T> constexpr T __vectorcall min(T a, T b) { return (a < b) ? a : b; }					//Returns the maximum value
template<class T> constexpr T __vectorcall max(T a, T b, T c) { return max(max(a, b), c); }				//Returns the minimum value
template<class T> constexpr T __vectorcall min(T a, T b, T c) { return min(min(a, b), c); }				//Returns the maximum value
template<class T> constexpr T __vectorcall max(T a, T b, T c, T d) { return max(max(max(a, b), c), d); }//Returns the minimum value
template<class T> constexpr T __vectorcall min(T a, T b, T c, T d) { return min(min(min(a, b), c), d); }//Returns the maximum value
template<class T> constexpr T __vectorcall swapVar(T a, T b) { return a ^= b ^= a ^= b; }				//Swaps the contents of 2 basic type variables


//Console output
#include "TermColor.hpp"
#define NewLine				std::cout						<< '\n';
#define Normal				std::cout<<termcolor::white		<< '\n';
#define Success				std::cout<<termcolor::green		<< '\n';
#define SuccessNoNl			std::cout<<termcolor::green;
#define Main				std::cout<<termcolor::magenta	<< '\n';
#define Failure				std::cout<<termcolor::red		<< '\n';
#define MainSeparator		Normal printf("\n\n#---------------------------------------------#\n\n");

#define LineInfo			Failure printf(">> %s, line %d, thread %d", __func__, __LINE__, std::this_thread::get_id())


#define TryVk(f)			if (f != VK_SUCCESS)
#define Exit(s)				{Failure printf("\nError:"); luxDebug(printf("\nFile %s", __FILE__)); printf("\nFunction %s, line %d:\n\n\"%s\"\n", __func__, __LINE__, s); Normal system("pause"); engine.running = false; exit(-1);}
#define luxDebug(s)			s
#define luxRelease()		;

#ifndef LUX_DEBUG
#define luxDebug()			;
#define luxRelease(s)		s
#endif


//Cause im lazy
#define null						nullptr
#define forEach(container, i)		for(uint64 i = 0; i < (container).size(); i++)
#define scast						static_cast
#define rcast						reinterpret_cast


//time
#include <chrono>
#include <thread>
typedef std::chrono::system_clock::time_point LuxTime;
#define luxStartChrono()			std::chrono::system_clock::now()
#define luxStopChrono(start)		((scast<std::chrono::duration<double>>(luxStartChrono() - start)).count())
#define sleep(ms)					std::this_thread::sleep_for(std::chrono::milliseconds(ms))










//TODO
//Calculates the square root of a float number. Sometimes this function is slightly slower than math.h sqrt function
static float __vectorcall __s(float n) {
	static int32_t i = 0x5F3759DF - (*(int32_t*)&n >> 1);
	static float n2 = *(float*)&i;
	return scast<float>(1 / (n2 * (1.5 - (n * 0.5 * n2 * n2))));
}

//Calculates the result of b to the power of e. Way faster than math.h pow function
static int64_t __vectorcall pow___(int64_t b, int64_t e) {
	int64_t r = 1;
	while (e > 0) {
		if (e & 1) r *= b % 1000000007;
		b *= b % 1000000007;
		e >>= 1;
	}
	return r;
}




#pragma warning(default : 4005) //Macro referinition
#pragma warning(default : 4002) //Too many arguments in luxDebug and luxRelease
