#pragma once
#if !defined __GNUC__ || !defined __cplusplus		//Use only gcc
#	error gcc compiler required
#endif


#ifdef _WIN64														//Windows
#	define win10(...) __VA_ARGS__										//Executes a line of code only when compiling for windows 10
#	define _linux(...)													//Executes a line of code only when compiling for linux
#	include <direct.h>													//  DIR | For _getcwd()
#	define __lp_get_cwd _getcwd											//  DIR | Get current working directory
#	define __lp_get_nopt(n)	n = std::thread::hardware_concurrency();	//  THR | Get number of physical threads
#	include <windows.h>													//  THR | For SuspendThread() and ResumeThread()
// #	define __lp_suspend_thr(th) SuspendThread(th)						//  THR | Function to suspend a thread
// #	define __lp_resume_thr(th)  ResumeThread(th)						//  THR | Function to resume a thread
#	define __lp_get_thr()  GetCurrentThread()							//  THR | Function to get the current thread ID

#elif defined __linux__												//Linux
#	define win10(...)													//Executes a line of code only when compiling for windows 10
#	define _linux(...) __VA_ARGS__										//Executes a line of code only when compiling for linux
#	include <unistd.h>													//  DIR | For getcwd()
#	define __lp_get_cwd getcwd											//  DIR | Get current working directory
#	define __lp_get_nopt(n) n = std::thread::hardware_concurrency();	//  THR | Get nuber of physical threads
#	include <pthread.h>													//  THR | For SuspendThread() and ResumeThread()
#	include <signal.h>													//  THR | For SuspendThread() and ResumeThread()
// #	define __lp_suspend_thr(th) pthread_kill(th, SIGSTOP)				//  THR | Function to suspend a thread
// #	define __lp_resume_thr(th)  pthread_kill(th, SIGCONT)				//  THR | Function to resume a thread
#	define __lp_get_thr()  gettid()										//  THR | Function to get the current thread ID

#else																//Other operating systems
#	error Unsupported operating system. Lux Engine can only run on windows 10 and linux
#endif
