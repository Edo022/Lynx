
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
		extern String thisDir;
		void fixWindowsPath(lux::String& pStr);
		lux::String getExtensionFromPath(const lux::String& pStr);
		lux::String getFileNameFromPath(const lux::String& pStr);
	}

	extern uint32 threadNum;
	void __lp_init_system();
}