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
#define Quit(s)				{Failure printf("\n%s\n", s); Normal system("pause"); exit(-1);}
#define MainSeparator		Normal printf("\n\n#---------------------------------------------#\n\n");


#define Try(f) if (f != VK_SUCCESS)
#define null nullptr


#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


//#include <chrono>
#define stdTime				std::chrono::system_clock::time_point
#define stdDuration			std::chrono::duration<double>
#define now					std::chrono::system_clock::now()

