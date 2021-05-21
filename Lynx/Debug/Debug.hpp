#pragma once
#define LNX_H_DEBUG
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/macros.hpp"
#include "Lynx/Debug/SourceInfo.hpp"
#include <cstdio>
#include <string>
#include <cstdarg>
#include <exception>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"
//TODO ADD FUNCTION TO GET FULL BACKTRACE AS RTARRAY OF HCARRAY
//TODO output to console window




namespace lnx::dbg{
	enum class Severity{
		eInfo,
		eWarning,
		eError
	};
	static std::string string_format(const std::string &fmt, ...) {
		int32 size = 4192;
		std::string str;
		va_list ap;

		while(true) {
			str.resize((u32)size);
			va_start(ap, fmt);
			int32 n = vsnprintf(&str[0], size, fmt.c_str(), ap);
			va_end(ap);

			if (n > -1 && n < size) {
				str.resize(n); // Make sure there are no trailing zero char
				return str;
			}
			if (n > -1) size = n + 1;
			else size *= 2;
		}
	}
	#ifdef LNX_DEBUG
		static neverInline void print(Severity vSeverity, const uint32 vIndex, const char* vMessage, const auto&... pParams) {
			//Create output string

			std::string out = string_format(
				"%s%s%s",
				"%s%s\n\n"		//Error
				"%s\"%s\"\n"	//Thread
				"%s%s\n\n",		//Traceback
				vMessage,		//User message
				"%s"
			);


			//Build traceback
			char thrName[16]; pthread_getname_np(pthread_self(), thrName, 16);
			std::string traceback = "\n    Address |   Line | Function";
			for(uint32 i = 0; ; ++i){
				auto func = caller::func(vIndex + i);
				if(func[0] != '?' && func[0] != '\0') {
					traceback += "\n    " +
					string_format("%7x", caller::addr(vIndex + i)) + " | " +
					string_format("%6d", caller::line(vIndex + i)) + " | " +
					func;
				}
				else break;
				if(i == LNX_CNF_DBG_MAX_BACKTRACE_DEPTH - 1) {
					traceback += "\n    Too many nested calls. Backtrace stopped";
					break;
				}
			}

			std::string out__ = string_format(out,
				"\n\n--------------------------------------------------------------------------\n",
				(vSeverity == Severity::eInfo) ? "" : (vSeverity == Severity::eWarning) ? "Warning" : "Error:",

				"Thread   ", thrName,
				"Traceback: ", traceback.c_str(),

				pParams...,
				"\n--------------------------------------------------------------------------\n\n"
			);
			if(vSeverity == Severity::eInfo) Normal else if(vSeverity == Severity::eWarning) Warning else Failure printf(out__.c_str());
			Normal; fflush(stdout); //free(out);
			if(vSeverity == Severity::eError) throw std::runtime_error("U.U");
		}

		static neverInline void printError  (const char* vMessage, const auto&... pParams) { print(Severity::eError  , 1, vMessage, pParams...); }
		static neverInline void printWarning(const char* vMessage, const auto&... pParams) { print(Severity::eWarning, 1, vMessage, pParams...); }








	//TODO add caller index and wrap args in a single parameter
		/**
		 * @brief Prints pMessage as error if vCond is true
		 */
		static neverInline void checkCond(const bool vCond, const char* pMessage, const auto&... pArgs) {
			if(vCond) lnx::dbg::printError(pMessage, pArgs...);
		}

		/**
		 * @brief Prints pMessage as error if vCond is true, specifying that a function parameter value is not valid
		 */
		static neverInline void checkParam(const bool vCond, const char* vParamName, const char* vMessage, const auto&... pParams) {
			if(vCond) {
				auto callerFunc = caller::func();
				const char* fstr = "Invalid value passed to \"%s\" parameter of function \"%s\":\n%s";
				char* str = (char*)malloc(strlen(fstr) + strlen(vParamName) + strlen(callerFunc) + strlen(vMessage) + 1);
				sprintf(str, fstr, vParamName, callerFunc, vMessage);
				lnx::dbg::printError(str, pParams...);
				free(str);
			}
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
		static neverInline void checkRawPtr(auto* vPtr, const char* vMessage, const auto&... vArgs) {
			if(vPtr) {
				try{ char tmp = *(char*)vPtr; }
				catch(std::exception& e) { dbg::printError(vMessage, vArgs...); }
			}
		}
	#else
		static alwaysInline void print(Severity, const uint32,       const char*, const auto&...) {}
		static alwaysInline void printError(                         const char*, const auto&...) {}
		static alwaysInline void printWarning(                       const char*, const auto&...) {}
		static alwaysInline void checkCond( const bool,              const char*, const auto&...) {}
		static alwaysInline void checkParam(const bool, const char*, const char*, const auto&...) {}
		static alwaysInline void checkVk(   const int,               const char*, const auto&...) {}
		static alwaysInline void checkIndex(const uint64, const uint64, const uint64, const char*) {}
		static alwaysInline void checkRawPtr(auto*, const char*, const auto&...) {}
	#endif
}



#pragma GCC diagnostic pop