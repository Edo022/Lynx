#include "Lynx/System/System.hpp"
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Core/AutoInit.hpp"








namespace lnx::sys{
	String dir::thisDir;	//Path to the current directory //Initialized in init function


	LnxAutoInit(LNX_H_SYSTEM) {
		char buff[FILENAME_MAX];					//Create char array to store the path
		__lp_get_cwd(buff, FILENAME_MAX);			//Get path
		dir::thisDir = buff;						//Save path
		_wds(dir::fixWindowsPath(dir::thisDir);)	//Replace silly windows backslashes with normal slashes
	}




	namespace dir{ //TODO
		//Replaces backslashes with normal slashes
		void fixWindowsPath(const String& pStr) {
			for (char8& i : pStr) {
				if (i == '\\') i = '/';
				//else if(i == ' ') i = '' //TODO spaces
			}
		}
	}
}



