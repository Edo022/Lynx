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
#define forEach(container, iterator) for(uint64 iterator = 0; iterator < container.size(); iterator++)
#define sc static_cast

//functions
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define swapVar(a,b)		a ^= b ^= a ^= b			//Swaps the contents of 2 basic type variables



//time
//#include <chrono>
#define stdTime				std::chrono::system_clock::time_point
#define stdDuration			std::chrono::duration<double>
#define now					std::chrono::system_clock::now()

