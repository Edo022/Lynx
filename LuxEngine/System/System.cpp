#include "LuxEngine/System/System.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux::sys{
	String dir::thisDir;	//Path to the current directory //Initialized in init function


	luxAutoInit(LUX_H_SYSTEM) {
		char buff[FILENAME_MAX];					//Create char array to store the path
		__lp_get_cwd(buff, FILENAME_MAX);			//Get path
		dir::thisDir = buff;						//Save path
		_wds(dir::fixWindowsPath(dir::thisDir);)	//Replace silly windows backslashes with normal slashes
	}








	namespace dir{
		//Replaces backslashes with normal slashes
		void fixWindowsPath(const String& pStr) {
			for (char8& i : pStr) {
				if (i == '\\') i = '/';
				//else if(i == ' ') i = '' //TODO spaces
			}
		}

		//Returns a lux::String containing the extension of the file
		//e.g. getExtensionFromPath("/home/dir/file.txt") returns "txt"
		//Directories or files with no exention returns ""
		lux::String getExtensionFromPath(const lux::String& pStr) {
			uint32 i = pStr.count() - 1;
			for(; pStr[i] != '.' && i > 0; --i) if(pStr[i] == '/') return lux::String("");
			return lux::String(pStr.begin() + i + 1);
		}

		//Returns a lux::String containing the name of the file
		//e.g. getFileNameFromPath("/home/dir/file.txt") returns "file.txt"
		lux::String getFileNameFromPath(const lux::String& pStr) {
			uint32 i = pStr.count() - 1;
			for(; pStr[i] != '/' && i > 0; --i);
			return lux::String(pStr.begin() + i + 1);
		}
	}
}



