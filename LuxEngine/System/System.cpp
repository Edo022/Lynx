#include "LuxEngine/System/System.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"

#include <new>
//TODO remove nothing constructors
//TODO remove LuxNoInit and VarNoInit macros








namespace lux::sys{
	String dir::thisDir;	//Path to the current directory //Initialized in init function
	//TODO move to lux::thr
	//FIXME idk what happened here.
	//FIXME threadNum should be initialized
	// uint32		threadNum = lux::sys::threadNum;						//Number of threads in the main CPU
	uint32		threadNum;						//Number of threads in the main CPU


	luxAutoInit(LUX_H_SYSTEM){
		static bool once = true;
		if(once) {								//Execute only once
			once = false;
			char buff[FILENAME_MAX];				//Create char array to store the path
			__lp_get_cwd(buff, FILENAME_MAX);		//Get path
			dir::thisDir = String(buff);		//Save path
			dir::fixWindowsPath(dir::thisDir);		//Replace silly windows backslashes with normal slashes
			__lp_get_nopt(threadNum);				//Get number of physical threads
		}
	}








	namespace dir{
		//Replaces backslashes with normal slashes
		void fixWindowsPath(String& pStr) {
			for (char8& i : pStr) {
				if (i == '\\') i = '/';
				//else if(i == ' ') i = '' //TODO spaces
			}
		}

		//Returns a lux::String containing the extension of the file
		//e.g. getExtensionFromPath("/home/dir/file.txt") returns "txt"
		//Directories or files with no exention returns ""
		lux::String getExtensionFromPath(const lux::String& pStr) {
			uint32 i = pStr.count( ) - 1;
			for(; pStr[i] != '.' && i > 0; --i) if(pStr[i] == '/') return lux::String("");
			return lux::String(pStr.begin( ) + i + 1);
		}

		//Returns a lux::String containing the name of the file
		//e.g. getFileNameFromPath("/home/dir/file.txt") returns "file.txt"
		lux::String getFileNameFromPath(const lux::String& pStr) {
			uint32 i = pStr.count( ) - 1;
			for(; pStr[i] != '/' && i > 0; --i);
			return lux::String(pStr.begin( ) + i + 1);
		}
	}
}



