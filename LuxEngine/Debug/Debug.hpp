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




	//Prints a warning, specifying the line, function, file and thread
	#define luxPrintWarning(...){																									\
		char __thrName__[16]; pthread_getname_np(pthread_self(), __thrName__, 16);													\
		Warning printf("Warning in thread %s, file %s\nfunction %s, line %d:", __thrName__, __FILE__, __FUNCTION__, __LINE__);		\
		Warning printf(__VA_ARGS__); fflush(stdout); NormalNoNl;																	\
	}




	//FIXME FIX DEBUG FUNCTIONS INLINING
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

	/**
	 * @brief Prints an error if vResult is not VK_SUCCESS
	 */
	template<class... types> static void checkVk(const int/*VkResult*/ vResult, const char* vMessage, types... vArgs) {
		checkCond(vResult != 0/*VK_SUCCESS*/, vMessage, vArgs...);
	}

	/**
	 * @brief Prints an error if the pointer is not valid.
	 *		This function is unreliable and very likely to not detect all the invalid pointers.
	 *		It should only be used as additional security check
	 */
	template<class pType, class... aTypes> static void checkRawPtr(pType* vPtr, const char* vMessage, aTypes... vArgs){
		if(vPtr){
			try{ char tmp = *(char*)vPtr; }
			catch(std::exception e) { dbg::printError(vMessage, vArgs...); }
		}
	}
}
