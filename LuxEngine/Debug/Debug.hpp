#pragma once
#define LUX_H_DEBUG
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Debug/SourceInfo.hpp"
#include <cstdio>
#include <cstring>
#include <exception>






//TODO output to console window
namespace lux::dbg{
	enum class Severity{
		info,
		warning,
		error
	};
	#ifdef LUX_DEBUG
		template<class... types> static neverInline void print(Severity vSeverity, const uint32 vIndex, const char* vMessage, const types&... pParams) {
			//Create output string

			const char* bgn =
				"%s%s\n\n"		//"Error"
				"%s\"%s\"\n"	//Thread
				"%s\"%s\"\n"	//File
				"%s\"%s\"\n"	//Function
				"%s%d\n\n"		//Line
			;
			const char* end = "%s";
			char* out = (char*)malloc(strlen(bgn) + strlen(vMessage) + strlen(end) + 1);
			sprintf(out, "%s%s%s", bgn, vMessage, end);

			//Output
			char thrName[16]; pthread_getname_np(pthread_self(), thrName, 16);
			char* out__ = (char*)malloc(8192); snprintf(out__, 8192, out,
				"\n\n--------------------------------------------------------------------------\n",
				(vSeverity == Severity::info) ? "" : (vSeverity == Severity::warning) ? "Warning" : "Error:",

				"Thread   ", thrName,
				"File     ", caller::file(vIndex + 1),
				"Function ", caller::func(vIndex + 1),
				"Line     ", caller::line(vIndex + 1),

				pParams...,
				"\n--------------------------------------------------------------------------\n\n"
			);
			if(vSeverity == Severity::info) Normal else if(vSeverity == Severity::warning) Warning else Failure printf(out__);
			Normal; fflush(stdout); free(out);
			if(vSeverity == Severity::error) throw std::runtime_error("U.U");
		}

		template<class... types> static neverInline void printError  (const char* vMessage, const types&... pParams) { print(Severity::error  , 1, vMessage, pParams...); }
		template<class... types> static neverInline void printWarning(const char* vMessage, const types&... pParams) { print(Severity::warning, 1, vMessage, pParams...); }








	//TODO add caller index and wrap args in a single parameter
		/**
		 * @brief Prints pMessage as error if vCond is true
		 */
		template<class... types> static neverInline void checkCond(const bool vCond, const char* pMessage, const types&... pArgs) {
			if(vCond) lux::dbg::printError(pMessage, pArgs...);
		}

		/**
		 * @brief Prints pMessage as error if vCond is true, specifying that a function parameter value is not valid
		 */
		template<class... types> static neverInline void checkParam(const bool vCond, const char* vParamName, const char* vMessage, const types&... pParams) {
			if(vCond) {
				auto callerFunc = caller::func();
				const char* fstr = "Invalid value passed to \"%s\" parameter of function \"%s\":\n%s";
				char* str = (char*)malloc(strlen(fstr) + strlen(vParamName) + strlen(callerFunc) + strlen(vMessage) + 1);
				sprintf(str, fstr, vParamName, callerFunc, vMessage);
				lux::dbg::printError(str, pParams...);
				free(str);
			}
		}

		/**
		 * @brief Prints pMessage as error if vResult is not VK_SUCCESS
		 */
		template<class... types> static neverInline void checkVk(const int/*VkResult*/ vResult, const char* vMessage, types... vArgs) {
			checkCond(vResult != 0/*VK_SUCCESS*/, vMessage, vArgs...);
		}

		/**
		 * @brief Prints pMessage as error if vIndex is not between vMin and vMax
		 */
		static neverInline void checkIndex(const uint64 vIndex, const uint64 vMin, const uint64 vMax, const char* vParamName) {
			checkParam(vIndex < vMin || vIndex > vMax, vParamName, "Index %llu is out of range. Min: %llu, Max: %llu", vIndex, vMin, vMax);
		}

		/**
		 * @brief Prints pMessage as error if the pointer is not valid.
		 *		This function is unreliable and very likely to not detect all the invalid pointers.
		*		It should only be used as additional security check
		*/
		template<class pType, class... aTypes> static neverInline void checkRawPtr(pType* vPtr, const char* vMessage, aTypes... vArgs) {
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wunused-variable"
			if(vPtr) {
				try{ char tmp = *(char*)vPtr; }
				catch(std::exception& e) { dbg::printError(vMessage, vArgs...); }
			}
			#pragma GCC diagnostic pop
		}
	#else
		template<class... t> static alwaysInline void print(Severity, const uint32, const char*, const t&...) {}
		template<class... t> static alwaysInline void printError  (const char*, const t&...) {}
		template<class... t> static alwaysInline void printWarning(const char*, const t&...) {}
		template<class... t> static alwaysInline void checkCond(const bool, const char*, const t&...) {}
		template<class... t> static alwaysInline void checkParam(const bool, const char*, const char*, const t&...) {}
		template<class... t> static alwaysInline void checkVk(const int, const char*, t...) {}
							 static alwaysInline void checkIndex(const uint64, const uint64, const uint64, const char*) {}
		template<class pType, class... aTypes> static alwaysInline void checkRawPtr(pType*, const char*, aTypes...) {}
	#endif
}
