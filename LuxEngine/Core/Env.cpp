#define LUX_NH_ENV
#include <cstdlib>
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/macros.hpp"


luxAutoInit(LUX_NH_ENV) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wwrite-strings"

    //! Can't use $VULKAN_SDK in putenv function
	// putenv(           "PATH=" enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/bin"); //TODO CHECK IF /usr/bin and /usr/bin64 ARE PRESENT IN PATH. ADD THEM IF NOT
	// putenv("LD_LIBRARY_PATH=" enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/lib");	//TODO USE LOCAL SHARED LIBRARY INSTEAD OF THE ONE ON THE USER'S SYSTEM //FIXME LD PATH NOT WORKING
	putenv("VK_LAYER_PATH=" enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/etc/vulkan/explicit_layer.d");
	putenv("VK_ICD_FILENAMES=" "/usr/share/vulkan/icd.d/radeon_icd.x86_64.json"); 		//FIXME check if this works with nvidia gpus //FIXME write variant for Windows
	//!^ Specify the driver to use to prevent Fedora 33 from using the lvp one
	//!^ The lvp_icd driver creates a lot of threads that usually crash the system or slow down everything

	#pragma GCC diagnostic pop
}
