#pragma once
#define LNX_H_SOURCE_INFO
#include "Lynx/Types/Integers/Integers.hpp"
#include <cstring>
#include <string>
#include <execinfo.h>
#include <cxxabi.h>
#include <fstream>
#include <cassert>








// Utilities (executable name, command outputs, backtrace) --------------------------------------------------------------------------------------------//








namespace lnx::dbg{
    /** <pre>
	 * @brief Returns the name of the executable file	\n
	 * Complexity: O(n)									\n
	 *     where n = length of the executable name in bytes
	 * @return The name of the executable file
	 </pre> */
	static const char* getExecName() {
		#ifdef _WIN64 //FIXME
			char name[MAX_PATH];						//Create name buffer
			GetModuleFileNameA(nullptr, name, MAX_PATH);//Get name
			return name;
		#else
			FILE* f = fopen("/proc/self/comm", "r");
			char* name = (char*)malloc(256);			//Create name buffer
			fgets(name, 256, f);						//Read name
			auto nameSize = strlen(name);				//Remove trailing \n
			if(name[nameSize - 1] == '\n') name[nameSize - 1] = '\0';
			fclose(f);
			return name;
		#endif
	}



	/** <pre>
	 * @brief Returns a string containing the output of a console command. Lines are separated by a '\n'							\n
	 *     ! Notice that this function is equivalent to running system(), which is system specific and spawns one or more processes	\n
	 *     ! Use subpOutput to get the output of a subprocess //TODO write subpOutput //TODO execl									\n
	 * Complexity: O(n + l + m)																										\n
	 *     with n = vCmd.size(), l = number of lines in the command output and m = length of the command output
	 * @param vCmd Command to execute
	 * @param vMaxLineLen Maximum length of each line of the output
	 * @return The command output
	 </pre> */
	static char* cmdOutput(std::string vCmd, const uint32 vMaxLineLen = 8192) {
		//Open console and run the command
		assert(vMaxLineLen < INT32_MAX);
		FILE* f = popen((vCmd + "\n").c_str(), "r");
		assert(f); fflush(f);

		//Create outpupt buffer and read the output
		char* line = (char*)calloc(vMaxLineLen, 1);
		std::string out;
		while(fgets(line, (i32)vMaxLineLen, f) != nullptr) {
			out += line;
		}
		pclose(f);

		//Return (+1 is for \0)
		line = (char*)realloc(line, out.length() + 1);
		memcpy(line, out.c_str(), out.length() + 1);
		return line;
	}




	/** <pre> //FIXME don't use addr2line but something more reliable instead. Like debug informations
	 * @brief Returns address, demangled function name, file name and line of the vIndex-th call in the backtrace	\n
	 *      ! This function only works on Linux systems. On Windows, "" is returned //TODO add default string		\n
	 *      ! Using this function in a Windows build will cause a warning to be displayed during compilation		\n
	 * Complexity: //TODO
	 * @param vIndex The index of the call to get informations about
	 *     0 is the function you are calling this from, 1 is its caller and so on
	 * @return //TODO
	 </pre> */
	static neverInline auto getBacktrace(uint32 vIndex, const bool vGetFunc = true) {
		++vIndex;									//Skip this call
		void* calls[vIndex + 1];					//Create address buffer
		backtrace(calls, (i32)vIndex + 1);			//Get calls addresses

		char* str = (char*)malloc(128);				//Create addr2line command (-1 is to get the actual instruction address)
		sprintf(str, "addr2line%s -e %s --demangle %p", (vGetFunc) ? " -f" : "", getExecName(), (void*)((char*)calls[vIndex] - 1));
		return cmdOutput(str);						//Get file infos from address and return
	}




	/** <pre>
	 * @brief Returns the address of the vIndex-th call in the backtrace \n
	 * Complexity: //TODO
	 * @param vIndex The index of the call to get the address of
	 * @return The address of the call
	 </pre> */
	static neverInline auto getBacktraceAddr(uint32 vIndex) {
		++vIndex;									//Skip this call
		void* calls[vIndex + 1];					//Create address buffer
		backtrace(calls, (i32)vIndex + 1);			//Get calls addresses
		return (void*)((char*)calls[vIndex] - 1);
	}








	// Get self or caller informations --------------------------------------------------------------------------------------------------------------//








    /** <pre>
     * @brief Retrieves informations about the caller of the function
     </pre> */
	class caller{
		public:
		/** <pre>
		 * @brief Returns the line of a function call
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 </pre> */
		static neverInline auto line(const uint32 vIndex = 1) {
			auto str = getBacktrace(vIndex + 1, false);
			if(str[0] == '\0') return 0;
			for(uint32 i = strlen(str); ; --i) if(str[i] == ':') {
				auto ret = atoi(str + i + 1);
				free(str);
				return ret;
			}
		}

		/** <pre>
		 * @brief Returns the name of a caller function
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 </pre> */
		static neverInline auto func(const uint32 vIndex = 1) {
			auto str = getBacktrace(vIndex + 1);
			if(str[0] == '\0') return str;
			for(uint32 i = 0; ; ++i) if(str[i] == '\n') { str[i] = '\0'; return str; };
			return str;
		}

		/** <pre>
		 * @brief Returns the name of the file where a caller function is located
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 </pre> */
		static neverInline auto file(const uint32 vIndex = 1) {
			auto str = getBacktrace(vIndex + 1, false);
			if(str[0] == '\0') return str;
			for(uint32 i = 0; ; ++i) if(str[i] == ':') { str[i] = '\0'; return str; };
			return str;
		}

		/** <pre>
		 * @brief Returns the address of the caller function
		 * @param vIndex The index of the caller. 0 is the function where this is used, 1 is its caller
		 </pre> */
		static neverInline auto addr(const uint32 vIndex = 1) {
			return getBacktraceAddr(vIndex + 1);
		}
	};








    /** <pre>
     * @brief Retrieves informations about the line of code you are writing in
     </pre> */
    class self{
        public:
		/** <pre>
		 * @brief Alias for __LINE__ or dbg::caller::file(0);
		 </pre> */
		static neverInline auto line() {
			return dbg::caller::line();
		}

		/** <pre>
		 * @brief Alias for __FUNCTION__, __func__ or dbg::caller::func(0);
		 </pre> */
		static neverInline auto func() {
            return dbg::caller::func();
        }

		/** <pre>
		 * @brief Alias for __FILE__ or dbg::caller::file(0)
		 </pre> */
		static neverInline auto file() {
            return dbg::caller::file();
		}

		/** <pre>
		 * @brief Returns the address of the current function
		 </pre> */
		static neverInline auto addr() {
            return dbg::caller::addr();
		}
    };
}