#pragma once
#define LUX_H_CONSOLE_OUTPUT
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Debug/SourceInfo.hpp"
#include <cstdio>
#include <cstring>







namespace lux::dbg{
	template<class... types> static void printError(const char* vMessage, const types&... pParams){
		//Create output string
		const char* bgn = "%s\n\n%s\n\n%s\"%s\"\n%s\"%s\"\n%s\"%s\"\n%s%d\n\n";
		const char* end = "\n\n%s";
		char* out = (char*)malloc(strlen(bgn) + strlen(out) + strlen(vMessage) + strlen(end) + 1);
		strcpy(out, bgn);
		strcat(out, vMessage);
		strcat(out, end);

		//Output
		char thrName[16]; pthread_getname_np(pthread_self(), thrName, 16);
		Failure printf(out,
			"###############################################################",

			"Lux runtime error:",

			"Thread   ", thrName,
			"File     ", caller::file(),
			"Function ", caller::func(),
			"Line     ", caller::line(),

			pParams...,

			"###############################################################"
		);
		fflush(stdout); Normal;
	}









	//Prints an error, specifying the line, function, file and thread where the error occurred
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	// #define dbg::printError(...)		checkCond(true, __VA_ARGS__)
	// #define dbg::printError(...)		lux::out::printError();/*BUG FIXME*/


	//Prints a warning, specifying the line, function, file and thread
	#define luxPrintWarning(...){																									\
		char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);													\
		Warning printf("Warning in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);		\
		Warning printf(__VA_ARGS__); fflush(stdout); NormalNoNl;																	\
	}








	/**
	 * @brief Prints pMessage as error if vCond is true
	 */
	template<class... types> static alwaysInline void checkCond(const bool vCond, const char* pMessage, const types&... pArgs) {
		if(vCond) lux::dbg::printError(pMessage, pArgs...);
	}

	/**
	 * @brief Prints an error specifying that a parameter value is not valid
	 */
	template<class... types> static void checkParam(const bool vCond, const char* vParamName, const char* vMessage, const types&... pParams) {
		if(vCond) {
			char* str = (char*)malloc(4192);
			sprintf(str, "Invalid value passed to \"%s\" parameter of function \"%s\":\n%s", vParamName, caller::func(), vMessage);
			lux::dbg::printError(str, pParams...);
		}
	}

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
			Failure printf(__VA_ARGS__); fflush(stdout); NormalNoNl /*__lux_out_stop__*/											\
		}																															\
	})

	/**
	 * @brief Prints an error if the pointer is not valid.
	 *		This function is unreliable and very likely to not detect all the invalid pointers.
	 *		It should only be used as additional security check
	 */
	template<class pType, class... aTypes> static void checkRawPtr(pType* vPtr, const char* vMessage, aTypes... vArgs){
		if(vPtr){
			try{ pType tmp = *vPtr; }
			catch(std::exception e) { dbg::printError(vMessage, vArgs...); }
		}
	}
}
