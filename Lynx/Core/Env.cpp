#define LNX_NH_ENV
#include <cstdlib>
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/macros.hpp"
#include <string>
#include <cstring>


LnxAutoInit(LNX_NH_ENV) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wwrite-strings"

	printf("DEBUG_3\n");
	fflush(stdout);

    //! Can't use $VULKAN_SDK in putenv function
	// putenv(           "PATH=" enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/bin"); //TODO CHECK IF /usr/bin and /usr/bin64 ARE PRESENT IN PATH. ADD THEM IF NOT
	// auto st = getenv("LD_LIBRARY_PATH");
	// printf("%s", st);

	printf("DEBUG_3a2\n");
	fflush(stdout);

	// auto s = std::string() + "LD_LIBRARY_PATH=" + getenv("LD_LIBRARY_PATH") + enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/lib";

	printf("DEBUG_3a\n");
	fflush(stdout);

	// char* e = (char*)malloc(s.length() + 1);

	printf("DEBUG_3b\n");
	fflush(stdout);

	// memcpy(e, s.c_str(), s.length() + 1);

	printf("DEBUG_3c\n");
	fflush(stdout);

	// putenv(e);	//TODO USE LOCAL SHARED LIBRARY INSTEAD OF THE ONE ON THE USER'S SYSTEM //FIXME LD PATH NOT WORKING
	// putenv("LIBRARY_PATH=" enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/lib");	//TODO USE LOCAL SHARED LIBRARY INSTEAD OF THE ONE ON THE USER'S SYSTEM //FIXME LD PATH NOT WORKING
	// printf("%s\n", getenv("LIBRARY_PATH"));

	printf("DEBUG_3d\n");
	fflush(stdout);

	putenv("VK_LAYER_PATH=" enginePath "/deps/Linux/Vulkan-1.2.170.0/x86_64/etc/vulkan/explicit_layer.d");
	printf("%s\n", getenv("VK_LAYER_PATH"));
	// putenv("VK_ICD_FILENAMES=" "/usr/share/vulkan/icd.d/radeon_icd.x86_64.json"); 		//FIXME check if this works with nvidia gpus //FIXME write variant for Windows
	//!^ Specify the driver to use to prevent Fedora 33 from using the lvp one
	//!^ The lvp_icd driver creates a lot of threads that usually crash the system or slow down everything
	//TODO LOAD IMPLICIT LAYER

	printf("DEBUG_4\n");
    fflush(stdout);
	//Lynx/deps/Linux/Vulkan-1.2.170.0/x86_64/lib/libVkLayer_khronos_validation.so
	//sudo cp Lynx/deps/Linux/Vulkan-1.2.170.0/x86_64/lib/libVkLayer_khronos_validation.so /lib64/
	#pragma GCC diagnostic pop
}
