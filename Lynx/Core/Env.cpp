#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h> //TODO write file test for windows
#include "Lynx/macros.hpp"
#include "Lynx/Core/Env.hpp"
#include "Lynx/Debug/Debug.hpp"
//TODO LOAD IMPLICIT LAYER
//TODO allow user to link the local library instead of the one on their system




namespace lnx{
	_lnx_init_fun_def(LNX_H_ENV, lnx) {
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wwrite-strings"


		char* layerPath = (char*)malloc(1024);
		sprintf(layerPath, "VK_LAYER_PATH=" enginePath "/Deps/Linux/Vulkan/explicit_layer.d");
		_dbg(putenv(layerPath)); //!explicit_layer.d is copy pasted from the SDK


		//TODO move to path config file or make it overwrite this env variable
		const char* paths[4] = { "/usr/local/etc/vulkan/icd.d", "/usr/local/share/vulkan/icd.d", "/etc/vulkan/icd.d", "/usr/share/vulkan/icd.d" };
		const char* icds [3] = { "intel_icd", "radeon_icd", "lvp_icd" };
		const char* ptfs [1] = { "x86_64" };
		char icdFilename[1024], *icdFilenames = (char*)malloc(4096);
		bool d = false;
		for(int i = 0; i < 4; ++i) for(int j = 0; j < 3; ++j) for(int k = 0; k < 1; ++k){
			sprintf(icdFilename, "%s/%s.%s.json", paths[i], icds[j], ptfs[k]);
			if(access(icdFilename, F_OK) == 0){
				strcat(icdFilenames, d ? ":" : "VK_ICD_FILENAMES=");
				strcat(icdFilenames, icdFilename);
				d = true;
			}
		}
		putenv(icdFilenames);


		#pragma GCC diagnostic pop
	}
}