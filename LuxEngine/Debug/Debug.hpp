#pragma once
#define LUX_H_CONSOLE_OUTPUT
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/macros.hpp"
#include <cstdio>
#include <cstring>
#include <execinfo.h>
#include <cxxabi.h>
#include <fstream>
#include <exception>







namespace lux::dbg{
	//Returns the name of the executable file
	static const char* getExecName(){
		#if defined(PLATFORM_POSIX) || defined(__linux__)
			FILE* f = fopen("/proc/self/comm", "r");
			char* name = (char*)malloc(256);		//Create name buffer
			fgets(name, 256, f);					//Read name
			auto nameSize = strlen(name);			//Remove trailing \n
			if(name[nameSize - 1] == '\n') name[nameSize - 1] = '\0';
			fclose(f);
			return name;
		#elif defined(_WIN64)
			char name[MAX_PATH];						//Create name buffer
			GetModuleFileNameA(nullptr, name, MAX_PATH);//Get name
			return name;
		#endif
	}



	//Returns a string containing the output lines of a console command
	static char* exec(const char* cmd, const int vMaxLineLen = 8192) {
		FILE* pipe = popen(cmd, "r");				//Open console and run the command
		if (!pipe) printf("Traceback error\n");		//Check for file validity

		uint32 outputSize = 0;
		char* output = (char*)malloc(8192);			//Create outpupt buffer and read the output
		while(fgets(output + outputSize, vMaxLineLen, pipe) != nullptr) {
			outputSize = strlen(output);
			output = (char*)realloc(output, vMaxLineLen + outputSize);
		}
		pclose(pipe);								//Close file
		return output;								//Return
	}



	//Returns the call file, function and line of the vIndex backtraced function
	//(0 is the function you are calling this from, 1 is its caller)
	static neverInline auto getBacktrace(int vIndex, bool vGetFunc = true){
		++vIndex; 												//Skip call to this function
		void* calls[vIndex + 1];									//Create address buffer
		auto size = backtrace(calls, vIndex + 1);		//Get calls addresses

		char* str = (char*)malloc(128);							//Create addr2line command (-1 is to get the actual instruction address)
		sprintf(str, "addr2line%s -e %s --demangle %x", (vGetFunc) ? " -f" : "", getExecName(), (char*)calls[vIndex] - 1);
		return exec(str);										//Get file infos from address and return
	}



	class caller{
		public:
		/**
		 * @brief Returns the line of a function call
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 */
		static neverInline auto line(const uint32 vIndex = 1){
			auto s = getBacktrace(vIndex + 1, false);
			for(uint32 i = strlen(s); ; --i) if(s[i] == ':') return atoi(s + i + 1); //TODO free s
		}
		/**
		 * @brief Returns the name of a caller function
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 */
		static neverInline auto func(const uint32 vIndex = 1){
			auto s = getBacktrace(vIndex + 1);
			for(uint32 i = 0; ; ++i) if(s[i] == '\n') { s[i] = '\0'; return s; };
			return s;
		}
		/**
		 * @brief Returns the name of the file where a caller function is located
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 */
		static neverInline auto file(const uint32 vIndex = 1){
			auto s = getBacktrace(vIndex + 1, false);
			for(uint32 i = 0; ; ++i) if(s[i] == ':') { s[i] = '\0'; return s; };
			return s;
		}
	};



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








	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//*   vCond: The condition to check
	//*   pMessage: printf format string
	//*   vArgs: printf parameters
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	template<class... types> static alwaysInline void checkCond(const bool vCond, const char* pMessage, const types&... pArgs) {
		if(vCond) lux::dbg::printError(pMessage, pArgs...);
	}
	//Prints an error if the condition is satisfied, specifying the line, function, file and thread where the error occurred
	//*   condition: The condition to check
	//*   param: The function parameter to check
	//*   __args: printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	template<class... types> static void alwaysInline checkParam(const bool vCond, const char* vVarName, const char* vMessage, const types&... pParams) {
		if(vCond) {
			char* str = (char*)malloc(4192);
			sprintf(str, "Invalid value passed to \"%s\" parameter of function \"%s\":\n%s", vVarName, caller::func(), vMessage);
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
			Failure printf(__VA_ARGS__); fflush(stdout); NormalNoNl /*__lux_out_stop__*/												\
		}																															\
	})
	//Prints an error if the pointer is unallocated or invalid. This check can still fail
	//nullptr is considered valid
	//*   ptr: The pointer to check
	//*   __type: The type of the pointer
	//*   __args: printf arguments to print the error
	//Debug mode only. #define LUX_PAUSE_ON_ERROR to make the program stop when an error occurs
	#define luxCheckRawPtr(ptr, __type, ...) luxRelease(;) luxDebug({			\
		if(ptr){																\
			try{ __type __var = *ptr; }											\
			catch(std::exception e) { dbg::checkParam(true, #ptr, __VA_ARGS__); }	\
		}																		\
	};)
}
