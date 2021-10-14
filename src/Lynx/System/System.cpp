#include "Lynx/System/System.hpp"
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Core/Init.hpp"








namespace lnx::sys{
	//Path to the current directory
	namespace dir {
		_lnx_init_var_value_def((String), thisDir, lnx::sys::dir){
			char buff[FILENAME_MAX];			//Create char array to store the path
			_lnx_get_cwd(buff, FILENAME_MAX);	//Get path
			*pVar = buff;						//Save path
			_wds(fixWindowsPath(*pVar);)			//Replace silly windows backslashes with normal slashes
		}
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



