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
#pragma GCC diagnostic ignored "-Wunused-function"

//TODO ADD FUNCTION TO GET FULL BACKTRACE AS RTARRAY OF HCARRAY
//TODO output to console window








// Structs and helper functions ---------------------------------------------------------------------------------------------------//








namespace lnx::dbg{
	/**
	 * @brief Formats an std::string using the printf format
	 *     This is an helper function and should only be called from lnx::dbg::print
	 * Complexity: ~O(n) [depends on the complexity of vsnprintf, which is unkown]
	 *     where n = pFstr.size()
	 * @param pFstr The format string to print (standard printf syntax)
	 * @param ... The format arguments
	 */
	static std::string string_format(const std::string &pFstr, ...) {
		int32 size = 4192;
		std::string str;
		va_list ap;

		while(true) {
			str.resize((u32)size);
			va_start(ap, pFstr);
			int32 n = vsnprintf(&str[0], (uint64)size, pFstr.c_str(), ap);
			va_end(ap);


			if(n > -1 && n < size) {
				str.resize((uint64)n); // Make sure there are no trailing '\0's
				return str;
			}
			else size *= 2;
		}
	}




	/**
	 * @brief Enum defining the severity of messages printed by the lnx::dbg::print function
	 */
	enum class Severity{
		eInfo,		//White  text, execution not stopped
		eWarn,		//Orange text, execution not stopped
		eError		//Red text, throws an exception in debug mode
	};








	// Print ----------------------------------------------------------------------------------------------------------------------//








	/** //FIXME use gray or white color for file links
	 * @brief Prints a message to the standard output, surrounding it with ---- separators and coloring it based on its severity
	 *     Errors throw exceptions when in debug mode
	 *     A maximum of LNX_CNF_DBG_MAX_BACKTRACE_DEPTH lines of backtrace is printed before the message
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::string_format && depends on traceback size]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param vSeverity The severity of the massage. Can be dbg::Severity::eInfo, dbg::Severity::eWarn or dbg::Severity::eError
	 * @param vIndex The index of the caller. //TODO explain how this works
	 * @param pFstr The format string to print as error (standard printf syntax)
	 * @param pArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void print(Severity vSeverity, const uint32 vIndex, const char* pFstr, const auto&... pArgs) {
		using namespace std::chrono;
		using std::string;
		#ifdef LNX_DEBUG
			//Get time
			time_t cTime; time(&cTime);				//Get current time
			tm *lTime; lTime = localtime(&cTime);	//Convert to local time
			auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) % seconds(1);

			//Format time
			string out = string_format(
				"[%02d:%02d:%02d.%03d] ",
				lTime->tm_hour,
				lTime->tm_min,
				lTime->tm_sec,
				ms.count()
			);

			if(vSeverity == Severity::eInfo){
				out = string_format(out + pFstr, pArgs...);
			}
			else{
				//Create output string
				out = string_format(
					string("\n\n") + out +
					"%s%s%s",		//Format
					"%s"			//Separator
					"%s\n\n"		//Severity
					"%s\"%s\"\n"	//Thread
					"%s%s\n\n",		//Traceback
					pFstr,			//User message
					"%s"			//Separator
				);


				//Build traceback
				char thrName[16]; pthread_getname_np(pthread_self(), thrName, 16);
				string traceback = "\n    Address |   Line | Function";
				for(uint32 i = 0; ; ++i){
					auto func = caller::func(vIndex + i);
					if(func[0] != '?' && func[0] != '\0') {
						traceback +=
							"\n    " +
							string_format("%7x", caller::addr(vIndex + i)) + " | " +
							string_format("%6d", caller::line(vIndex + i)) + " | " +
							func +
							" [" + caller::file(vIndex + i) + ":" +
							string_format("%d", caller::line(vIndex + i)) + "]"
						;
					}
					else break;
					if(i == LNX_CNF_DBG_MAX_BACKTRACE_DEPTH - 1) {
						traceback += "\n    Too many nested calls. Backtrace stopped";
						break;
					}
				}


				//Merge default text and user data
				// std::string out__ = string_format(out,
				out = string_format(out,
					"------------------------------------------------------------\n",
					(vSeverity == Severity::eInfo) ? "" : (vSeverity == Severity::eWarn) ? "Warning" : "Error:",

					"Thread   ", thrName,
					"Traceback: ", traceback.c_str(),

					pArgs...,
					"\n--------------------------------------------------------------------------\n\n"
				);
			}


			//Print
			if(vSeverity == Severity::eInfo) Normal else if(vSeverity == Severity::eWarn) Warning else Failure;
			// printf(out__.c_str()); Normal; fflush(stdout);
			printf(out.c_str()); NormalNoNl; fflush(stdout);
			if(vSeverity == Severity::eError) throw std::runtime_error("U.U");
		#endif
	}




	/**
	 * @brief Alias for dbg::print(dbg::Severity::eError, 1, pFstr, pArgs)
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::print]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param pFstr The format string to print as error (standard printf syntax)
	 * @param pArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void logError(const char* pFstr, const auto&... pArgs) {
		#ifdef LNX_DEBUG
			print(Severity::eError, 1, pFstr, pArgs...);
		#endif
	}


	/**
	 * @brief Alias for dbg::print(dbg::Severity::eWarn, 1, pFstr, pArgs)
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::print]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param pFstr The format string to print as warning (standard printf syntax)
	 * @param pArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void logWarn(const char* pFstr, const auto&... pArgs) {
		#ifdef LNX_DEBUG
			print(Severity::eWarn, 1, pFstr, pArgs...);
		#endif
	}


	/**
	 * @brief Alias for dbg::print(dbg::Severity::eInfo, 1, pFstr, pArgs)
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::print]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param pFstr The format string to print (standard printf syntax)
	 * @param pArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void logInfo(const char* pFstr, const auto&... pArgs) {
		#ifdef LNX_DEBUG
			print(Severity::eInfo, 1, pFstr, pArgs...);
		#endif
	}








	// Checks ---------------------------------------------------------------------------------------------------------------------//








//TODO add caller index and wrap args in a single parameter
//TODO change checks to print if the condition is false, like assert
	/**
	 * @brief Prints an error if vCond is true
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::logError]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param vCond The condition to meet
	 * @param pFstr The format string to print as error (standard printf syntax)
	 * @param pArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void checkCond(const bool vCond, const char* pFstr, const auto&... pArgs) {
		#ifdef LNX_DEBUG
			if(vCond) lnx::dbg::logError(pFstr, pArgs...);
		#endif
	}




	/**
	 * @brief Prints an error if vCond is true
	 *     Specifies that the value of the function parameter with name vParam is not valid
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::logError && depends on caller function name]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param vCond The condition to meet
	 * @param pParam The name of the parameter
	 * @param pFstr The format string to print as error (standard printf syntax)
	 * @param pArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void checkParam(const bool vCond, const char* pParam, const char* pFstr, const auto&... pArgs) {
		#ifdef LNX_DEBUG
			if(vCond) {
				auto callerFunc = caller::func();
				const char* fstr = "Invalid value passed to \"%s\" parameter of function \"%s\":\n%s";
				char* str = (char*)malloc(strlen(fstr) + strlen(pParam) + strlen(callerFunc) + strlen(pFstr) + 1);
				sprintf(str, fstr, pParam, callerFunc, pFstr);
				lnx::dbg::logError(str, pArgs...);
				free(str);
			}
		#endif
	}




	/**
	 * @brief Prints an error if vIndex is not between vMin and vMax
	 *     Specifies the minimum and maximum value the function parameter with name pParam can have
	 *     Indices are printed as uint64
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::checkParam]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param vIndex The index to check
	 * @param vMin The minimum allowed value of the index
	 * @param vMax The maximum allowed value of the index
	 * @param pParam The name of the parameter
	 */
	static _dbg(neverInline)_rls(alwaysInline) void checkIndex(const uint64 vIndex, const uint64 vMin, const uint64 vMax, const char* pParam) {
		#ifdef LNX_DEBUG
			checkParam(vIndex < vMin || vIndex > vMax, pParam, "Index %llu is out of range. Min: %llu, Max: %llu", vIndex, vMin, vMax);
		#endif
	}




	/**
	 * @brief Prints pMessage as error if the pointer is not valid
	 *		! Notice that this function is unreliable and very likely to not detect all the invalid pointers
	 *		! It should only be used as additional safety test when writing debug checks to provide additional infos other than "segmentation fault"
	 * Complexity:
	 *     ~O(n) [debug mode, from dbg::logError]
	 *     O(1)  [release mode, function call is ignored]
	 *     where n = strlen(pFstr)
	 * @param vPtr The pointer to check
	 * @param pFstr The format string to print as error (standard printf syntax)
	 * @param vArgs The format arguments
	 */
	static _dbg(neverInline)_rls(alwaysInline) void checkRawPtr(auto* vPtr, const char* pFstr, const auto&... vArgs) {
		#ifdef LNX_DEBUG
			if(vPtr) {
				try{ char tmp = *(char*)vPtr; }
				catch(std::exception& e) { dbg::logError(pFstr, vArgs...); }
			}
		#endif
	}
}



#pragma GCC diagnostic pop