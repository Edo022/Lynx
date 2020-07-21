
#include "LuxEngine/System/System.h"


lux::String lux::sys::dir::thisDir = "";
uint32		lux::sys::threadNum = 0;


namespace lux::sys{
	namespace dir{
		//Replaces backslashes with normal slashes
		void fixWindowsPath(lux::String& pStr) {
			for (auto& i : pStr) {
				if (i == '\\') i = '/';
				//else if(i == ' ') i = '' //TODO spaces
			}
		}

		//Returns a lux::String containing the extension of the file
		//e.g. getExtensionFromPath("/home/dir/file.txt") returns "txt"
		//Directories or files with no exention returns ""
		lux::String getExtensionFromPath(const lux::String& pStr) {
			uint32 i = pStr.size( ) - 1;
			for(; pStr[i] != '.' && i > 0; --i) if(pStr[i] == '/') return lux::String("");
			return lux::String(pStr.begin( ) + i + 1);
		}

		//Returns a lux::String containing the name of the file
		//e.g. getFileNameFromPath("/home/dir/file.txt") returns "file.txt"
		lux::String getFileNameFromPath(const lux::String& pStr) {
			uint32 i = pStr.size( ) - 1;
			for(; pStr[i] != '/' && i > 0; --i);
			return lux::String(pStr.begin( ) + i + 1);
		}

	}
}