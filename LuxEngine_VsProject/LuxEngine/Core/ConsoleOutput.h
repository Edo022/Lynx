#pragma once
//#include "LuxEngine/Types/Containers/LuxString.h"
#include <string>
#include <stdio.h>
#include "LuxEngine/macros.h"
#include <exception>



namespace lux::out{
	void _printError(const std::string& pMessage, const bool vFatalError = false, const int32 vErrorCode = -1);
	void _printWarning(const std::string& pMessage);
	void __stop__();

	//#define printError(pMessage, vFatalError, vErrorCode) lux::out::_printError(std::string("Function ").append(__FUNCTION__).append(", line ").append(std::to_string((unsigned long long)__LINE__)).append("\n\n").append(pMessage), vFatalError, vErrorCode)
	#define printError(pMessage, vFatalError, vErrorCode) lux::out::_printError(std::string("Function ").append("h").append(", line ").append(std::to_string((unsigned long long)__LINE__)).append("\n\n").append(pMessage), vFatalError, vErrorCode)
	//#define printWarning(pMessage) lux::out::_printError(std::string("Function ").append(__FUNCTION__).append(", line ").append(std::to_string(__LINE__)).append("\n\n").append(pMessage))
	#define printWarning(pMessage) lux::out::_printError(std::string("Function ").append("h").append(", line ").append(std::to_string(__LINE__)).append("\n\n").append(pMessage))

	#define lux_error(condition, ...)\
		luxDebug(										\
		if(condition) {																		\
			Failure printf("Error in function %s, line %d:\n", "h", __LINE__);				\
			/*Failure printf("Error in function %s, line %d:\n", __FUNCTION__, __LINE__);		*/\
			Failure printf(__VA_ARGS__);											\
			/*throw std::exception("UwU");													*/\
			lux::out::__stop__();\
		}																					\
	)
	#define param_error_2(condition, param, ...) luxDebug(													\
		if(condition) {																								\
			Failure printf("Error in function %s, line %d:\n", "h", __LINE__);								\
			/*Failure printf("Error in function %s, line %d:\n", __FUNCTION__, __LINE__);								*/\
			Failure printf("Invalid value passed to \"%s\" parameter of function \"%s\".\n", #param, "h");	\
			/*Failure printf("Invalid value passed to \"%s\" parameter of function \"%s\".\n", #param, __FUNCTION__);	*/\
			Failure printf(__VA_ARGS__);																	\
			/*throw std::exception("UwU");																			*/\
			lux::out::__stop__();\
		}																											\
	)
	#define ptr_validity(ptr, __type, ...) { luxDebug(						\
		if(ptr){																	\
			try{ __type __var = *ptr; }												\
			catch(std::exception e) { param_error(ptr, __VA_ARGS__); }		\
		}																			\
	)}



	#define param_error(paramName, errorInfo) luxDebug(																				\
		printError(std::string("Invalid value passed to '").append(#paramName).append("' parameter of function '").append("h").append("'\n")		\
		/*printError(std::string("Invalid value passed to '").append(#paramName).append("' parameter of function '").append(__FUNCTION__).append("'\n")	*/\
		.append(errorInfo).append("\nThis error will not be reported in release mode"), true, -1)												\
	) luxRelease(;)
}
