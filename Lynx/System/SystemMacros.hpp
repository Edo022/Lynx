#pragma once
////#define LNX_H_SYSTEM_MACROS
#include <thread>




#if !defined __GNUC__ || !defined __cplusplus		//Use only g++
    #error g++ compiler required
#endif


#ifdef _WIN64														//Windows
    #define _wds(...) __VA_ARGS__										//Executes a line of code only when compiling for windows 10
    #define _lnx(...)													//Executes a line of code only when compiling for linux
    #include <direct.h>													//  DIR | For _getcwd()
    #define __lp_get_cwd _getcwd											//  DIR | Get current working directory
    // #include <windows.h>													//  THR | For SuspendThread() and ResumeThread()
    // #define __lp_suspend_thr(th) SuspendThread(th)						//  THR | Function to suspend a thread
    // #define __lp_resume_thr(th)  ResumeThread(th)						//  THR | Function to resume a thread

#elif defined __linux__												//Linux
    #define _wds(...)													//Executes a line of code only when compiling for windows 10
    #define _lnx(...) __VA_ARGS__										//Executes a line of code only when compiling for linux
    #include <unistd.h>													//  DIR | For getcwd()
    #define __lp_get_cwd getcwd											//  DIR | Get current working directory
    // #include <pthread.h>													//  THR | For SuspendThread() and ResumeThread()
    // #include <csignal>													//  THR | For SuspendThread() and ResumeThread()
    // #define __lp_suspend_thr(th) pthread_kill(th, SIGSTOP)				//  THR | Function to suspend a thread
    // #define __lp_resume_thr(th)  pthread_kill(th, SIGCONT)				//  THR | Function to resume a thread

#else																//Other operating systems
    #error Unsupported operating system. The Lynx Engine can only run on windows 10 and linux
#endif
