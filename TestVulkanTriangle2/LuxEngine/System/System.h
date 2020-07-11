
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






namespace lux::System {
	//The path to the current directory
	extern lux::String luxThisDirectory;


	//Replaces backslashes with normal slashes
	static void fixWindowsPath(lux::String& pStr) {
		for (auto& i : pStr) {
			if (i == '\\') i = '/';
			//else if(i == ' ') i = '' //TODO spaces
		}
	}


	static inline void __lp_get_current_working_directory() {
		static bool once = true;
		if (once) {
			char buff[FILENAME_MAX];				//Create char array to store the path
			__lp_get_cwd(buff, FILENAME_MAX);		//Get path
			luxThisDirectory = lux::String(buff);		//Save path
			lux::System::fixWindowsPath(luxThisDirectory);	//Replace silly windows backslashes with normal slashes
		}
	}


	//Returns a lux::String containing the extension of the file
	//e.g. getExtensionFromPath("/home/dir/file.txt") returns "txt"
	//Directories or files with no exention returns ""
	static lux::String getExtensionFromPath(const lux::String& pStr) {
		uint32 i; for (i = pStr.size() - 1; pStr[i] != '.' && i > 0; --i) {
			if (pStr[i] == '/') return lux::String("");
		}
		return lux::String(pStr.begin() + i + 1);
	}


	//Returns a lux::String containing the name of the file
	//e.g. getFileNameFromPath("/home/dir/file.txt") returns "file.txt"
	static lux::String getFileNameFromPath(const lux::String& pStr) {
		uint32 i; for (i = pStr.size() - 1; pStr[i] != '/' && i > 0; --i);
		return lux::String(pStr.begin() + i + 1);
	}
}