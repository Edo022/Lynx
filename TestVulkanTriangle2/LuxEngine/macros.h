#pragma once


//#include <chrono>
//#include <thread>
#define sleep(ms)			std::this_thread::sleep_for(std::chrono::milliseconds(ms))

//#include "TermColor.hpp"
//Console output
#define NewLine				std::cout						<< "\n";
#define Normal				std::cout<<termcolor::white		<< "\n";
#define Success				std::cout<<termcolor::green		<< "\n";
#define SuccessNoNl			std::cout<<termcolor::green;
#define Main				std::cout<<termcolor::magenta	<< "\n";
#define Failure				std::cout<<termcolor::red		<< "\n";
#define MainSeparator		Normal printf("\n\n#---------------------------------------------#\n\n");

#define Try(f) if (f != VK_SUCCESS)
#define Quit(s)				{Failure printf("\n%s\n", s); Normal system("pause"); exit(-1);}




#define null nullptr
#define forEach(container, iterator)	for(uint64 iterator = 0; iterator < container.size(); iterator++)
#define sc static_cast

//functions
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define swapVar(a,b)		a ^= b ^= a ^= b			//Swaps the contents of 2 basic type variables



//time
//#include <chrono>
#define stdTime				std::chrono::system_clock::time_point
#define stdDuration			std::chrono::duration<double>
#define stdNow					std::chrono::system_clock::now()



static float s(float n) {
	static int32_t i = 0x5F3759DF - (*(int32_t*)&n >> 1);
	static float n2 = *(float*)&i;
	return 1 / (n2 * (1.5 - (n * 0.5 * n2 * n2)));
}


#define MOD 1000000007
static int64_t pow_(int64_t b, int64_t e) {
	int64_t r = 1;
	while (e > 0) {
		if (e & 1) r *= b % MOD;
		b *= b % MOD;
		e >>= 1;
	}
	return r;
}