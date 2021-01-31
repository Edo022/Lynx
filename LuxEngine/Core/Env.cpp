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
	putenv((s(     "VULKAN_SDK=") + getEnginePath() + "/deps/Linux/Vulkan-1.2.162.0/x86_64"                            ).data());
	putenv((s(           "PATH=") + getEnginePath() + "/deps/Linux/Vulkan-1.2.162.0/x86_64/bin"                        ).data());
	putenv((s("LD_LIBRARY_PATH=") + getEnginePath() + "/deps/Linux/Vulkan-1.2.162.0/x86_64/lib"                        ).data());
	putenv((s(  "VK_LAYER_PATH=") + getEnginePath() + "/deps/Linux/Vulkan-1.2.162.0/x86_64/etc/vulkan/explicit_layer.d").data());
	#pragma GCC diagnostic pop
}
