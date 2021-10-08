#include <cstdlib>
#include <string>
#include <unistd.h>
#include <cstring>
#include "Lynx/macros.hpp"
#include "Lynx/Core/Env.hpp"
#include "Lynx/Debug/Debug.hpp"
//TODO LOAD IMPLICIT LAYER
//TODO allow user to link the local library instead of the one on their system




namespace lnx{
	_lnx_init_fun_def(LNX_H_ENV, lnx) {
		char* layerPath = (char*)malloc(1024);
		sprintf(layerPath, "VK_LAYER_PATH=" enginePath "/Deps/Linux/Vulkan/explicit_layer.d");
		_dbg(putenv(layerPath)); //!explicit_layer.d is copy pasted from the SDK


		//TODO move to path config file or make it overwrite this env variable
		std::string paths[4] = { "/usr/local/etc/vulkan/icd.d", "/usr/local/share/vulkan/icd.d", "/etc/vulkan/icd.d", "/usr/share/vulkan/icd.d" };
		std::string icds [3] = { "intel_icd", "radeon_icd", "lvp_icd" };
		std::string ptfs [1] = { "x86_64" };
		// char icdFilename[1024], *icdFilenames = (char*)malloc(4096);
		std::string icdFilenames;
		bool d = false;
		for(int i = 0; i < 4; ++i) for(int j = 0; j < 3; ++j) for(int k = 0; k < 1; ++k){
			// sprintf(icdFilename, "%s/%s.%s.json", paths[i], icds[j], ptfs[k]);
			auto icdFilename = paths[i] + "/" + icds[j] + "." + ptfs[k] + ".json";
			if(access(icdFilename.c_str(), F_OK) == 0){
				// strcat(icdFilenames, d ? ":" : "VK_ICD_FILENAMES=");
				icdFilenames += d ? ":" : "VK_ICD_FILENAMES=";
				icdFilenames += icdFilename;
				// strcat(icdFilenames, icdFilename);
				d = true;
			}
		}
		//TODO optimize
		char* cIcdFilenames = (char*)malloc(icdFilenames.length() + 1);
		strcpy(cIcdFilenames, icdFilenames.c_str());
		putenv(cIcdFilenames);
		printf("ICDS: %s\n", cIcdFilenames);
	}
}