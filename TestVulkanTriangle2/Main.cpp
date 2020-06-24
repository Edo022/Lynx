
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include <filesystem>
#include <iostream>
#include <stdio.h>



#if !defined _MSC_VER || !defined __cplusplus		//Use only C++ visual studio compiler
#	error Visual studio C++ compiler required
#endif


#	ifdef _WIN64							//Windows
#		include <direct.h>						//Windows directories
#		define __lp_get_cwd _getcwd				//Windows directories
#	elif defined __linux__					//Linux
#		include <unistd.h>						//Linux directories
#		define __lp_get_cwd getcwd				//Linux directories
#	else									//Other operating systems
#		error Unsupported operating system
//#	elif defined unix || defined __unix || define __unix__	//Unix 
#	endif


//Rrturns the path to the current directory as a LuxString
static inline LuxString luxGetCurrentWorkingDirectory() {
	char buff[FILENAME_MAX];
	__lp_get_cwd(buff, FILENAME_MAX);
	return LuxString(buff);
}




int main() {
	printf("%s", luxGetCurrentWorkingDirectory().begin());


	LuxInit(false);
	LuxInputState inputStateTest{
		LuxKeySequence{ {LUX_KEY_W | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode)->void {  printf("AAAAA"); }},
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode)->void {  printf("BBBB"); } } ,
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode)->void {  printf("CCC"); } },
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode)->void {  printf("DD"); } }
	};
	luxInputSetInputState(&inputStateTest);


	while (engine.running) {
		//int h = -573289;
		//printf("%lf\n", fcos(h));
		sleep(1);
	}
	return 0;
}