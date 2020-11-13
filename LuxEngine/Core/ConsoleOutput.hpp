#pragma once
//#include "LuxEngine/Types/Containers/LuxString.h"
#include <string>
#include <stdio.h>
#include "LuxEngine/macros.hpp"
#include <exception>



namespace lux::out{
	void _printError(const std::string& pMessage, const bool vFatalError = false, const int32 vErrorCode = -1);		//TODO REMOVE
	void _printWarning(const std::string& pMessage);																//TODO REMOVE
	void __stop__();


	//TODO REMOVE
	#define printError(pMessage, vFatalError, vErrorCode) lux::out::_printError(std::string("Function ").append("h").append(", line ").append(std::to_string((unsigned long long)__LINE__)).append("\n\n").append(pMessage), vFatalError, vErrorCode)
	#define printWarning(pMessage) lux::out::_printError(std::string("Function ").append("h").append(", line ").append(std::to_string(__LINE__)).append("\n\n").append(pMessage))



	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//Debug mode only
	//*   condition | The condition to check
	//*   __args ---| printf arguments to print the error
	#define luxCheckCond(condition, ...) luxDebug({																					\
		if(condition) {																												\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf(__VA_ARGS__);																							\
			NormalNoNl lux::out::__stop__();																						\
		}																															\
	)}
	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//Debug mode only
	//*   condition | The condition to check
	//*   param ----| The function parameter to check
	//*   __args ---| printf arguments to print the error
	#define luxCheckParam(condition, param, ...) luxDebug({																			\
		if(condition) {																												\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf("Invalid value passed to \"%s\" parameter of function \"%s\": %s\n", #param, __FUNCTION__, __VA_ARGS__);	\
			NormalNoNl lux::out::__stop__();																						\
		}																															\
	)}
	//Prints an error if the vulkan function does not return VK_SUCCESS, specifying the line, function, file and thread where the error occurred
	//Debug mode only
	//*   funcCall | The call to the vulkan function to check
	//*   __args --| printf arguments to print the error
	#define luxCheckVk(funcCall, ...) luxDebug({																					\
		uint64 callRes = funcCall; const char* callStr = #funcCall;																	\
		if(callRes != VK_SUCCESS) {																									\
			char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);												\
			Failure printf("Error in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);	\
			Failure printf("The Vulkan function call \"%s\" returned %d", callStr, callRes);										\
			Failure printf(__VA_ARGS__);																							\
			NormalNoNl lux::out::__stop__();																						\
		}																															\
	)}
	//Prints an error if the pointer is unallocated or invalid. This check can still fail
	//nullptr is considered valid
	//*   ptr ---| The pointer to check
	//*   __type | The type of the pointer
	//*   __args | printf arguments to print the error
	//Debug mode only
	#define luxCheckRawPtr(ptr, __type, ...) luxDebug({							\
		if(ptr){																\
			try{ __type __var = *ptr; }											\
			catch(std::exception e) { luxCheckParam(true, ptr, __VA_ARGS__); }	\
		}																		\
	)}


	//TODO REMOVE
	#define param_error(paramName, errorInfo) luxDebug(																				\
		printError(std::string("Invalid value passed to '").append(#paramName).append("' parameter of function '").append("h").append("'\n")		\
		.append(errorInfo).append("\nThis error will not be reported in release mode"), true, -1)												\
	) luxRelease(;)
}
