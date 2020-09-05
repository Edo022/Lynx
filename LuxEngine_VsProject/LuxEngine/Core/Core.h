
#pragma once
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Types/LuxBool.h"
#include "vulkan/vulkan.h"









/*
Object rendering


																							   Render space assembler
																										¦
																										¦
																										¦
																			  Render space assembler    ¦
																						 ¦              ¦
																						 ¦              ¦     Render space assembler
																						 ¦              ¦              ¦
																	SortByZIndex()       ¦              ¦              ¦
								  Objects cache                     ToGlobalPos()        ¦              ¦              ¦
								  .───────────────────.                                  ¦              ¦          .───¦───────────────.
								   \     .─────────.   \                                 ¦              ¦           \  ¦  .─────────.   \
		  Object 0 --> Draw() ----> \     \_________\   \ ----------> --------------> ---¦ ------------------------> \     \_________\   \
		  Ozindex = 2                \   Layer 0         \                               ¦                            \    Layer 0        \
								  .───\                   \                              ¦              ¦  .───────────\                   \
								   \   \___________________\                             ¦              ¦   \          ¦\___________________\
		  Object 1 --> Draw() ----> \                    \ ---------> ---.     .----> ---¦ ----------------> \   Layer ¦ 2  \____\
		  zindex = 0              .──\ Layer 1            \               '. .'          ¦                    \        ¦          \
								   \  \____________________\                X            ¦              ¦      \___________________\───.
									\  Layer 2     \____\                 .' '.          ¦              ¦          \   ¦                \
		  Object 2 --> Draw() -----> \                   \ ---------> ---'     '----> ---¦ -----------------------> \                    \
		  zindex = 1                  \___________________\                              ¦                           \ Layer 1            \
																						 ¦              ¦             \____________________\
																						 ¦              ¦              ¦
																						 ¦              ¦              ¦
																						 ¦              ¦              ¦
	i input                                                                              ¦              ¦              ¦
	u update                                                                             ¦              ¦              ¦
	l draw loop                                                                          ¦              ¦              ¦
	r read, w write                                                                      ¦              ¦              ¦
																						 ¦              ¦              ¦
   2D object                                                                             ¦    .         ↓              ¦              .
	   i, u | in object data | w SHARED                                                  ¦     \ Render space 2        ¦               \
	   i, u | Render object  | r SHARED, w VRAM                                      _ __¦ _    \______________________¦ _______________\                                            Window output                                 RS2
	   l    | Display cache  | w VRAM                                                    ¦  \                          ¦                                                              ___________________________________________________________________
																						 ↓   \     Render space 0      ¦                                                             │            ___________    │ /                                 │   │
   3D object                                                                        Render    \    ____________________¦ _____________________                                       │           │           │   │___________________________________│   │
	   i, u | in object data | w SHARED --> VRAM                                     space 1   \   \                   ↓                      \                                      │   _____   │ /         │                                           │
	   l    | Render object  | w VRAM                                                           \   \      .─ ─ ─ ─ ─ ─ ─ ─ ─ ─.               \                                     │  │     │  │ /         │    ____________________________________   │
																								 \   \      \      .─ ─ ─ ─ ─ ─ ─ ─ ─ ─.        \                                    │  │ /   │  │ /         │   │                                    │  │
																								  \   \      \      \     .─ ─ ─ ─ ─.   \        \  ------> CopyBuffer() --------->  │  │ /   │  │ /         │   │     .───────────────────.          │  │
																								   \   \      \      \     \_ _ _ _ _\   \        \                                  │  │ /   │  │ /         │   │     │      .───────────────────.   │  │
																									\   \      \ _ _ _\                   \        \                                 │  │ /   │  │ /         │   │     │   Lay│     .─────────.   │   │  │
																									 \   \          \  \                   \        \                                │  │ /   │  │ /         │   │     │      │     │_________│   │   │  │
																									  \   \          \  \_ _ _ _ _ _ _ _ _ _\        \                               │  │ /   │  │ /         │   │     │ _____│    Layer 0        │   │  │
																									   \   \          \ _ _ _ _ _ _ _ _ _ _\          \                              │  │ /   │  │           │   │         │  │                   │   │  │
																										\   \                                          \                             │  │ /   │  │           │   │         │ L│___________________│   │  │
																										 \   \                                          \                            │  │     │  │           │   │         │____________________│     │  │
																									_ ____\   \__________________________________________\                           │  │     │  │           │   │                                    │  │
																																													 │  │_____│  │___________│   │____________________________________│  │
																																													 │___________________________________________________________________│
																																														  RS3          RS1                         RS0



*/









namespace lux::core{
	extern double	FPS;
	extern float	FOV;
	extern bool		running;
	extern bool		useVSync;
	extern bool		initialized;
	extern uint32	frames;

	extern VkInstance					instance;
	extern VkDebugUtilsMessengerEXT		debugMessenger;
	extern VkSurfaceKHR					surface;

	extern Array<const char*, uint32>	validationLayers;
	extern Array<const char*, uint32>	requiredDeviceExtensions;




	void preInit( );
	void init(bool useVSync);
	struct LuxEngineInitializer{ LuxEngineInitializer( ){ lux::core::preInit( ); } };
	extern LuxEngineInitializer luxEngineInitializer; //This variable is used to inizialize the engine before any other variable or function call

	void run(bool vUseVSync, float vFOV);
	void mainLoop( );
	void runFPSCounterThr( );
	void runRenderThr( );


	void _printError(const String& pMessage, const bool vFatalError = false, const int32 vErrorCode = -1);
	void _printWarning(const String& pMessage);
	#define printError(pMessage, vFatalError, vErrorCode) lux::core::_printError(lux::String("Function ") + __FUNCTION__ + ", line " + __LINE__ + "\n\n" + (pMessage), vFatalError, vErrorCode)
	#define printWarning(pMessage) lux::core::_printError(lux::String("Function ") + __FUNCTION__ + ", line " + __LINE__ + "\n\n" + (pMessage))

	#define param_error(paramName, errorInfo) luxDebug(																				\
		printError(lux::String("Invalid value passed to '") + (#paramName) + "' parameter of function '" + __FUNCTION__ + "'\n"		\
		+ (errorInfo) + "\nThis error will not be reported in release mode", true, -1)												\
	) luxRelease(;)




	namespace debug {
		//It's dark magic, idk why or how it works, but it does
		inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if(func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			else return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
		inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if(func != nullptr) func(instance, debugMessenger, pAllocator);
		}


		//More dark magic
		static constexpr inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
			createInfo = { };
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			luxDebug(createInfo.pfnUserCallback = core::g::debugCallback);
		}
	}
}









//TODO fast memcpy codes
//#include <immintrin.h>
//#include <assert.h>
//#include <cstdint>
///* ... */
//void fastMemcpy(void *pvDest, void *pvSrc, size_t nBytes) {
//  assert(nBytes % 32 == 0);
//  assert((intptr_t(pvDest) & 31) == 0);
//  assert((intptr_t(pvSrc) & 31) == 0);
//  const __m256i *pSrc = reinterpret_cast<const __m256i*>(pvSrc);
//  __m256i *pDest = reinterpret_cast<__m256i*>(pvDest);
//  int64_t nVects = nBytes / sizeof(*pSrc);
//  for (; nVects > 0; nVects--, pSrc++, pDest++) {
//    const __m256i loaded = _mm256_stream_load_si256(pSrc);
//    _mm256_stream_si256(pDest, loaded);
//  }
//  _mm_sfence();
//}


//"
//On Ryzen 1800X with single memory channel filled completely (2 slots, 16 GB DDR4 in each), the following code is 1.56 times faster than memcpy() on MSVC++2017 compiler.
//If you fill both memory channels with 2 DDR4 modules, i.e. you have all 4 DDR4 slots busy, you may get further 2 times faster memory copying.
//For triple-(quad-)channel memory systems, you can get further 1.5(2.0) times faster memory copying if the code is extended to analogous AVX512 code.
//With AVX2-only triple/quad channel systems with all slots busy are not expected to be faster because to load them fully you need to load/store more than 32 bytes at once
//(48 bytes for triple- and 64-bytes for quad-channel systems), while AVX2 can load/store no more than 32 bytes at once.
//Though multithreading on some systems can alleviate this without AVX512 or even AVX2.
//
//So here is the copy code that assumes you are copying a large block of memory whose size is a multiple of 32 and the block is 32-byte aligned.
//
//For non-multiple size and non-aligned blocks, prologue/epilogue code can be written reducing the width to 16 (SSE4.1), 8, 4, 2 and finally 1 byte at once for the block head and tail.
//Also in the middle a local array of 2-3 __m256i values can be used as a proxy between aligned reads from the source and aligned writes to the destination.
//"



//TODO
//Interpolated point mesh rig