#pragma once
////#define LNX_H_DEBUG
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/macros.hpp"
#include "Lynx/Debug/SourceInfo.hpp"
#include <cstdio>
#include <string>
#include <cstdarg>
#include <exception>

// #include <sys/ioctl.h> //TODO MOVE TO SYSTEM INFO
//TODO add shell information to system info

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wunused-function"

//TODO ADD FUNCTION TO GET FULL BACKTRACE AS RTARRAY OF HCARRAY
//TODO output to console window








// Structs and helper functions ---------------------------------------------------------------------------------------------------//








namespace lnx::dbg{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-variable"
	inline namespace color {
		static const char* nBlack   = "\033[0;30m";
		static const char* nRed     = "\033[0;31m";
		static const char* nGreen   = "\033[0;32m";
		static const char* nYellow  = "\033[0;33m";
		static const char* nBlue    = "\033[0;34m";
		static const char* nMagenta = "\033[0;35m";
		static const char* nWhite   = "\033[0;37m";

		static const char* bBlack   = "\033[1;30m";
		static const char* bRed     = "\033[1;31m";
		static const char* bGreen   = "\033[1;32m";
		static const char* bYellow  = "\033[1;33m";
		static const char* bBlue    = "\033[1;34m";
		static const char* bMagenta = "\033[1;35m";
		static const char* bWhite   = "\033[1;37m";
	}
	#pragma GCC diagnostic pop

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







//TODO dont use the fancy output when printing infos
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
		#ifdef LNX_DBG
			// //Get terminal size //TODO MOVE TO SYSTEM INFO
			// struct winsize wsize;
    		// ioctl(0, TIOCGWINSZ, &wsize);

			//Get time
			time_t cTime; time(&cTime);				//Get current time
			tm *lTime; lTime = localtime(&cTime);	//Convert to local time
			auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) % seconds(1);


			//Build traceback
			string traceback = "\n    Address │   Line │ Function";
			for(uint32 i = 0; ; ++i){
				auto func = caller::func(vIndex + i);
				if(func[0] != '?' && func[0] != '\0') {
					// auto tracebackLine =
					traceback +=
						std::string("\n    ") +
						string_format(std::string(bBlue) + "%7x" + nWhite, caller::addr(vIndex + i)) + " │ " +
						string_format(std::string(bBlue) + "%6d" + nWhite, caller::line(vIndex + i)) + " │ " +
						func +
						bBlack + "  [" + caller::file(vIndex + i) + ":" + string_format("%d", caller::line(vIndex + i)) + "]" + nWhite;
					;
					// auto fileLink = std::string("[") + caller::file(vIndex + i) + ":" + string_format("%d", caller::line(vIndex + i)) + "]";

					// tracebackLine.resize(max(wsize.ws_col - fileLink.length()), ' ');
					// traceback += tracebackLine + fileLink;
				}
				else break;
				if(i == LNX_CNF_DBG_MAX_BACKTRACE_DEPTH - 1) {
					traceback += "\n    Too many nested calls. Backtrace stopped";
					break;
				}
			}


			//Build output string
			char thrName[16]; pthread_getname_np(pthread_self(), thrName, 16);
			std::string outCol  = (vSeverity == Severity::eInfo) ? bWhite : (vSeverity == Severity::eWarn) ? bYellow    : bRed;
			std::string msgType = (vSeverity == Severity::eInfo) ? ""     : (vSeverity == Severity::eWarn) ? "Warning"  : "Error:";
			std::string out = string_format(
				outCol + "[%02d:%02d:%02d.%03d] ────────────────────────────────────────────────────────────────────────────────────────────" +
				"\n" + msgType + nWhite +
				"\n" +
				"\nThread \"" + thrName + "\"" +
				"\nTraceback: " + traceback.c_str() +
				"\n" +
				"\n" + outCol + pFstr +
				"\n──────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n" + nWhite,
				lTime->tm_hour, lTime->tm_min, lTime->tm_sec, ms.count(), pArgs...
			);


			printf("%s\n", out.c_str()); fflush(stdout);
			if(vSeverity == Severity::eError) throw std::runtime_error("uwu");
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
		#ifdef LNX_DBG
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
		#ifdef LNX_DBG
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
		#ifdef LNX_DBG
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
		#ifdef LNX_DBG
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
		#ifdef LNX_DBG
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
		#ifdef LNX_DBG
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
		#ifdef LNX_DBG
			if(vPtr) {
				try{ char tmp = *(char*)vPtr; }
				catch(std::exception& e) { dbg::logError(pFstr, vArgs...); }
			}
		#endif
	}
}



#pragma GCC diagnostic pop