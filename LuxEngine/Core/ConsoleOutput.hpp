#pragma once
#define LUX_H_CONSOLE_OUTPUT
#include <cstring>
#include <cstdio>
#include "LuxEngine/macros.hpp"
#include <exception>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>







namespace lux::out{
	// #define LUX_PAUSE_ON_ERROR
	#if defined LUX_DEBUG && defined LUX_PAUSE_ON_ERROR
		#define __lux_out_stop__ throw std::out_of_range("UwU");
	#else
		#define __lux_out_stop__
	#endif



	static auto getBacktrace(){
        int *array[512];
        auto size = backtrace((void**)array, 512);

        char* str = (char*)malloc(512);
        for(auto i = 0; i < size; ++i) {
            sprintf(str, "addr2line -f -e a.out --demangle %x", array[i]);
            system(str); printf("\n");
        }
	}

	template<class... types> static void printError(char* message = "UwU", const types... params, callerInfo){
		//Create output string
		const char* bgn = "%s\n\n%s\n\n%s\"%s\"\n%s\"%s\"\n%s\"%s\"\n%s%d\n\n";
		const char* end = "\n\n%s";
		char* out = (char*)malloc(strlen(bgn) + strlen(out) + strlen(message) + strlen(end) + 1);
		strcpy(out, bgn);
		strcat(out, message);
		strcat(out, end);

		//Output
		char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);
		Failure printf(out,
			"###############################################################",

			"Lux runtime error:",

			"Thread   ", __thrName__,
			"File     ", caller.file_name(),
			"Function ", caller.function_name(),
			"Line     ", caller.line(),

			params...,

			"###############################################################"
		);
		fflush(stdout); Normal;
	}









	//Prints an error, specifying the line, function, file and thread where the error occurred
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	// #define luxPrintError(...)		checkCond(true, __VA_ARGS__)
	#define luxPrintError(...)		lux::out::printError();/*BUG FIXME*/


	//Prints a warning, specifying the line, function, file and thread
	#define luxPrintWarning(...){																									\
		char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);													\
		Warning printf("Warning in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);		\
		Warning printf(__VA_ARGS__); fflush(stdout); NormalNoNl;																	\
	}








	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//*   vCond: The condition to check
	//*   pMessage: printf format string
	//*   vArgs: printf parameters
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	template<class... types> static forceInline void checkCond(const bool vCond, const char* pMessage, const types... vArgs, callerInfo) {
		if(vCond) lux::out::printError<types...>((const char*)pMessage, vArgs..., caller);
	}
	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//*   condition: The condition to check
	//*   param: The function parameter to check
	//*   __args: printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckParam(condition, param, ...) luxRelease(;) luxDebug({															\
		if(condition) {																												\
			/*char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												*/\
			/*Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	*/\
			/*Failure printf("Invalid value passed to \"%s\" parameter of function \"%s\": %s\n", #param, __FUNCTION__, __VA_ARGS__);	*/\
			/*fflush(stdout); NormalNoNl __lux_out_stop__																				*/\
			lux::out::printError();/*BUG FIXME*/\
		}																															\
	})
	//Prints an error if the vulkan function does not return VK_SUCCESS, specifying the line, function, file and thread where the error occurred
	//*   funcCall: The call to the vulkan function to check
	//*   __args: printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckVk(funcCall, ...) luxRelease(;) luxDebug({																		\
		uint64 callRes = funcCall; const char* callStr = #funcCall;																	\
		if(callRes != VK_SUCCESS) {																									\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf("The Vulkan function call \"%s\" returned %d", callStr, callRes);										\
			Failure printf(__VA_ARGS__); fflush(stdout); NormalNoNl __lux_out_stop__												\
		}																															\
	})
	//Prints an error if the pointer is unallocated or invalid. This check can still fail
	//nullptr is considered valid
	//*   ptr: The pointer to check
	//*   __type: The type of the pointer
	//*   __args: printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckRawPtr(ptr, __type, ...) luxRelease(;) luxDebug({			\
		if(ptr){																\
			try{ __type __var = *ptr; }											\
			catch(std::exception e) { luxCheckParam(true, ptr, __VA_ARGS__); }	\
		}																		\
	};)
}
