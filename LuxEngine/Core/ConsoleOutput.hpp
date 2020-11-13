#pragma once
#define LUX_H_CONSOLE_OUTPUT
#include <string>
#include <stdio.h>
#include "LuxEngine/macros.hpp"
#include <exception>








namespace lux::out{
	#if defined LUX_DEBUG && defined LUX_PAUSE_ON_ERROR
		#define __lux_out_stop__ throw std::out_of_range("UwU");
	#else
		#define __lux_out_stop__
	#endif




	//Prints an error, specifying the line, function, file and thread where the error occurred
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxPrintError(...)		luxCheckCond(true, __VA_ARGS__)


	//Prints a warning, specifying the line, function, file and thread
	#define luxPrintWarning(...) 	luxDebug({																						\
		char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);													\
		Warning printf("Warning in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);		\
		Warning printf(__VA_ARGS__); NormalNoNl;																					\
	)}








	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//*   condition | The condition to check
	//*   __args ---| printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckCond(condition, ...) luxDebug({																					\
		if(condition) {																												\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf(__VA_ARGS__); NormalNoNl __lux_out_stop__																\
		}																															\
	)}
	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//*   condition | The condition to check
	//*   param ----| The function parameter to check
	//*   __args ---| printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckParam(condition, param, ...) luxDebug({																			\
		if(condition) {																												\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf("Invalid value passed to \"%s\" parameter of function \"%s\": %s\n", #param, __FUNCTION__, __VA_ARGS__);	\
			NormalNoNl __lux_out_stop__																								\
		}																															\
	)}
	//Prints an error if the vulkan function does not return VK_SUCCESS, specifying the line, function, file and thread where the error occurred
	//*   funcCall | The call to the vulkan function to check
	//*   __args --| printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckVk(funcCall, ...) luxDebug({																					\
		uint64 callRes = funcCall; const char* callStr = #funcCall;																	\
		if(callRes != VK_SUCCESS) {																									\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf("The Vulkan function call \"%s\" returned %d", callStr, callRes);										\
			Failure printf(__VA_ARGS__); NormalNoNl __lux_out_stop__																\
		}																															\
	)}
	//Prints an error if the pointer is unallocated or invalid. This check can still fail
	//nullptr is considered valid
	//*   ptr ---| The pointer to check
	//*   __type | The type of the pointer
	//*   __args | printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckRawPtr(ptr, __type, ...) luxDebug({							\
		if(ptr){																\
			try{ __type __var = *ptr; }											\
			catch(std::exception e) { luxCheckParam(true, ptr, __VA_ARGS__); }	\
		}																		\
	)}
}
