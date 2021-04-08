#pragma once
#define LUX_H_SOURCE_INFO
#include "Lynx/Types/Integers/Integers.hpp"
#include <cstring>
#include <execinfo.h>
#include <cxxabi.h>
#include <fstream>
#include <cassert>





namespace lux::dbg{
    /**
	 * @brief Returns the name of the executable file
	 */
	static const char* getExecName() {
		#if defined(PLATFORM_POSIX) || defined(__linux__)
			FILE* f = fopen("/proc/self/comm", "r");
			char* name = (char*)malloc(256);			//Create name buffer
			fgets(name, 256, f);						//Read name
			auto nameSize = strlen(name);				//Remove trailing \n
			if(name[nameSize - 1] == '\n') name[nameSize - 1] = '\0';
			fclose(f);
			return name;
		#elif defined(_WIN64)
			char name[MAX_PATH];						//Create name buffer
			GetModuleFileNameA(nullptr, name, MAX_PATH);//Get name
			return name;
		#endif
	}



	/**
	 * @brief Returns a string containing the output of a console command. Lines are separated by a '\n'
	 * @param vCmd Command to execute
	 * @param vMaxLineLen Maximum length of each line of the output
	 */
	static char* cmdOutput(const char* vCmd, const uint32 vMaxLineLen = 8192) {
		assert(vMaxLineLen < INT32_MAX);
		FILE* f = popen(vCmd, "r");				//Open console and run the command
		if (!f) printf("Traceback error\n");	//Check for file validity

		uint32 outputSize = 0;
		char* output = (char*)malloc(8192);		//Create outpupt buffer and read the output
		while(fgets(output + outputSize, (i32)vMaxLineLen, f) != nullptr) {
			outputSize = strlen(output);
			output = (char*)realloc(output, vMaxLineLen + outputSize);
		}
		pclose(f);								//Close file
		return output;							//Return
	}



	/**
	 * @brief Returns the call file, function and line of the vIndex backtraced function
	 *		(0 is the function you are calling this from, 1 is its caller)
	 */
	static neverInline auto getBacktrace(uint32 vIndex, const bool vGetFunc = true) {
		++vIndex;									//Skip this call
		void* calls[vIndex + 1];					//Create address buffer
		backtrace(calls, (i32)vIndex + 1);				//Get calls addresses

		char* str = (char*)malloc(128);				//Create addr2line command (-1 is to get the actual instruction address)
		sprintf(str, "addr2line%s -e %s --demangle %p", (vGetFunc) ? " -f" : "", getExecName(), (void*)((char*)calls[vIndex] - 1));
		return cmdOutput(str);						//Get file infos from address and return
	}








	// Get self or caller informations --------------------------------------------------------------------------------------------------------------//








    /**
     * @brief Retrieves informations about the caller of the function
     */
	class caller{
		public:
		/**
		 * @brief Returns the line of a function call
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 */
		static neverInline auto line(const uint32 vIndex = 1) {
			auto str = getBacktrace(vIndex + 1, false);
			for(uint32 i = strlen(str); ; --i) if(str[i] == ':') {
				auto ret = atoi(str + i + 1);
				free(str);
				return ret;
			}
		}

		/**
		 * @brief Returns the name of a caller function
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 */
		static neverInline auto func(const uint32 vIndex = 1) {
			auto str = getBacktrace(vIndex + 1);
			for(uint32 i = 0; ; ++i) if(str[i] == '\n') { str[i] = '\0'; return str; };
			return str;
		}

		/**
		 * @brief Returns the name of the file where a caller function is located
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 */
		static neverInline auto file(const uint32 vIndex = 1) {
			auto str = getBacktrace(vIndex + 1, false);
			for(uint32 i = 0; ; ++i) if(str[i] == ':') { str[i] = '\0'; return str; };
			return str;
		}
	};




    /**
     * @brief Retrieves informations about the line of code you are writing in
     */
    class self{
        public:
		/**
		 * @brief Alias for __LINE__ or dbg::caller::file(0);
		 */
		static neverInline auto line() {
			return dbg::caller::line();
		}

		/**
		 * @brief Alias for __FUNCTION__, __func__ or dbg::caller::func(0);
		 */
		static neverInline auto func() {
            return dbg::caller::func();
        }

		/**
		 * @brief Alias for __FILE__ or dbg::caller::file(0)
		 */
		static neverInline auto file() {
            return dbg::caller::file();
		}
    };
}