#define LUX_NH_ENV
#include <stdlib.h>
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/macros.hpp"
#include <string>

luxAutoInit(LUX_NH_ENV) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wwrite-strings"
	using s = std::string;

    //! Can't use $VULKAN_SDK in putenv function
	putenv(     "VULKAN_SDK=" enginePath "/deps/Linux/Vulkan-1.2.162.0/x86_64"                            );
	putenv(           "PATH=" enginePath "/deps/Linux/Vulkan-1.2.162.0/x86_64/bin"                        );
	putenv("LD_LIBRARY_PATH=" enginePath "/deps/Linux/Vulkan-1.2.162.0/x86_64/lib"                        );
	putenv(  "VK_LAYER_PATH=" enginePath "/deps/Linux/Vulkan-1.2.162.0/x86_64/etc/vulkan/explicit_layer.d");

	#pragma GCC diagnostic pop
}
