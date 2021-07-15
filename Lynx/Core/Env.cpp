#define LNX_NH_ENV
#include <cstdlib>
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/macros.hpp"
#include <string>
#include <cstring>
//TODO LOAD IMPLICIT LAYER




LnxAutoInit(LNX_NH_ENV) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wwrite-strings"

	// putenv("LIBRARY_PATH=" enginePath "/Deps/Linux/Vulkan-1.2.170.0/x86_64/lib");
	//TODO^ allow user to link the local library instead of the one on their system //FIXME^ LD PATH NOT WORKING
	_dbg(putenv("VK_LAYER_PATH=" enginePath "/Deps/Linux/Vulkan-1.2.170.0/x86_64/etc/vulkan/explicit_layer.d"));

	#pragma GCC diagnostic pop
}
