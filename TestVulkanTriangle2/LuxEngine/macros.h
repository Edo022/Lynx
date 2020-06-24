#pragma once
#pragma warning(disable : 4005) //Macro referinition
#pragma warning(disable : 4002) //Too many arguments in luxDebug and luxRelease
#include "LuxEngine.config.h"


#include <chrono>
//#include <thread>
#define sleep(ms)			std::this_thread::sleep_for(std::chrono::milliseconds(ms))
//#define Sleep(ms)			sleep(ms) //UwU use "sleep(ms)" instead of this
//#define _sleep(ms)			sleep(sms) //UwU use "sleep(ms)" instead of this

//#include "TermColor.hpp"
//Console output
#define NewLine				std::cout						<< '\n';
#define Normal				std::cout<<termcolor::white		<< '\n';
#define Success				std::cout<<termcolor::green		<< '\n';
#define SuccessNoNl			std::cout<<termcolor::green;
#define Main				std::cout<<termcolor::magenta	<< '\n';
#define Failure				std::cout<<termcolor::red		<< '\n';
#define MainSeparator		Normal printf("\n\n#---------------------------------------------#\n\n");

#define LineInfo			Failure printf(">> %s, line %d, thread %d", __func__, __LINE__, std::this_thread::get_id())


#define TryVk(f)			if (f != VK_SUCCESS)
#define Exit(s)				{Failure printf("\nError:\nFile %s\nFunction %s, line %d:\n\n\"%s\"\n", __FILE__, __func__, __LINE__, s); Normal system("pause"); engine.running = false; exit(-1);} //TODO dont use additional infos in release
#define luxDebug(s)			s
#define luxRelease()		;

#ifndef LUX_DEBUG
#define luxDebug()			;
#define luxRelease(s)		s
#endif



#define null nullptr
#define forEach(container, iterator)	for(uint64 iterator = 0; iterator < (container).size(); iterator++)
#define sc static_cast

//functions
#define max(a,b)            (((a) > (b)) ? (a) : (b))	//Don't use this macro with random numbers
#define min(a,b)            (((a) < (b)) ? (a) : (b))	//Don't use this macro with random numbers
#define swapVar(a,b)		a ^= b ^= a ^= b			//Swaps the contents of 2 basic type variables



//time
//#include <chrono>
typedef std::chrono::system_clock::time_point LuxTime;
#define luxGetTime()				std::chrono::system_clock::now()
#define luxTimeGetDuration(start)	((sc<std::chrono::duration<double>>(luxGetTime() - start)).count())


//Calculates the square root of a float number. Sometimes this function is slightly slower than math.h sqrt function
static float __vectorcall __s(float n) {
	static int32_t i = 0x5F3759DF - (*(int32_t*)&n >> 1);
	static float n2 = *(float*)&i;
	return sc<float>(1 / (n2 * (1.5 - (n * 0.5 * n2 * n2))));
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
//#pragma warning(default : 4002) //Too many arguments in luxDebug and luxRelease
