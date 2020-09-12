#pragma once
//#include "LuxEngine/Types/Containers/LuxString.h"
#include <string>



namespace lux::out{
	void _printError(const std::string& pMessage, const bool vFatalError = false, const int32 vErrorCode = -1);
	void _printWarning(const std::string& pMessage);


	#define printError(pMessage, vFatalError, vErrorCode) lux::out::_printError(std::string("Function ").append(__FUNCTION__).append(", line ").append(std::to_string((unsigned long long)__LINE__)).append("\n\n").append(pMessage), vFatalError, vErrorCode)
	#define printWarning(pMessage) lux::out::_printError(std::string("Function ").append(__FUNCTION__).append(", line ").append(std::to_string(__LINE__)).append("\n\n").append(pMessage))

	#define param_error(paramName, errorInfo) luxDebug(																				\
		printError(std::string("Invalid value passed to '").append(#paramName).append("' parameter of function '").append(__FUNCTION__).append("'\n")		\
		.append(errorInfo).append("\nThis error will not be reported in release mode"), true, -1)												\
	) luxRelease(;)
}
