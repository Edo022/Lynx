
#pragma once
#include <filesystem>
#include <stdio.h>
#include <thread>
#include "LuxEngine/Types/Containers/LuxString.h"




#if !defined _MSC_VER || !defined __cplusplus		//Use only C++ visual studio compiler
#	error Visual studio C++ compiler required
#endif


#ifdef _WIN64														//Windows
#	include <direct.h>													//  DIR | For _getcwd()
#	define __lp_get_cwd _getcwd											//  DIR | Get current working directory
#	define __lp_get_nopt(n)	n = std::thread::hardware_concurrency();	//  THR | Get number of physical threads
#	include <windows.h>													//  THR | For SuspendThread() and ResumeThread()
#	define __lp_suspend_thr(th) SuspendThread(th)						//  THR | Function to suspend a thread
#	define __lp_resume_thr(th) ResumeThread(th)							//  THR | Function to resume a thread
															
#elif defined __linux__												//Linux
#	include <unistd.h>													//  DIR | For getcwd()
#	define __lp_get_cwd getcwd											//  DIR | Get current working directory
#	define __lp_get_nopt(n) sysconf(n = _SC_NPROCESSORS_ONLN);			//  THR | Get nuber of physical threads
//  TODO																//  THR | For SuspendThread() and ResumeThread() // TODO tkill
//  TODO																//  THR | Function to suspend a thread
//  TODO																//  THR | Function to resume a thread
#	error use tkill - TODO - thread pool unimplemented 

#else																//Other operating systems
#	error Unsupported operating system
//#	elif defined unix || defined __unix || define __unix__	//Unix 
#endif








namespace lux::sys {
	namespace dir {
		//Path to the current directory
		extern lux::String thisDir;


		//Replaces backslashes with normal slashes
		static void fixWindowsPath(lux::String& pStr) {
			for (auto& i : pStr) {
				if (i == '\\') i = '/';
				//else if(i == ' ') i = '' //TODO spaces
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





	//Number of physical threads
	extern uint32 threadNum;


	static void __lp_init_system() {
		static bool once = true;
		if (once) {								//Execute only once
			char buff[FILENAME_MAX];				//Create char array to store the path
			__lp_get_cwd(buff, FILENAME_MAX);		//Get path
			dir::thisDir = lux::String(buff);		//Save path
			dir::fixWindowsPath(dir::thisDir);		//Replace silly windows backslashes with normal slashes

			__lp_get_nopt(threadNum);				//Get number of physical threads
		}
		once = false;
	}
}