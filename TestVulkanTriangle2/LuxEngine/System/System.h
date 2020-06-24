
#pragma once
#include <filesystem>
#include <stdio.h>
#include "LuxEngine/Types/Containers/LuxString.h"




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
static inline LuxString __lp_luxGetCurrentWorkingDirectory() {
	char buff[FILENAME_MAX];
	__lp_get_cwd(buff, FILENAME_MAX);
	return LuxString(buff);
}
static LuxString luxCurrentWorkingDirectory = __lp_luxGetCurrentWorkingDirectory();

