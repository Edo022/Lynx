
#pragma once
#include <filesystem>
#include <stdio.h>
#include "LuxEngine/Types/Containers/LuxString.h"




#if !defined _MSC_VER || !defined __cplusplus		//Use only C++ visual studio compiler
#	error Visual studio C++ compiler required
#endif


#ifdef _WIN64							//Windows
#	include <direct.h>						//Windows directories
#	define __lp_get_cwd _getcwd				//Windows directories
#elif defined __linux__					//Linux
#	include <unistd.h>						//Linux directories
#	define __lp_get_cwd getcwd				//Linux directories
#else									//Other operating systems
#	error Unsupported operating system
//#	elif defined unix || defined __unix || define __unix__	//Unix 
#endif



//The path to the current directory
extern LuxString luxThisDirectory;

static inline void __lp_lux_get_current_working_directory() {
	static bool once = true;
	if (once) {
		char buff[FILENAME_MAX];				//Create char array to store the path
		__lp_get_cwd(buff, FILENAME_MAX);		//Get path
		luxThisDirectory = LuxString(buff);		//Save path
		luxFixWindowsPath(luxThisDirectory);	//Replace silly windows backslashes with normal slashes
	}
}

