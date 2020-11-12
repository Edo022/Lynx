
#pragma once
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/Graphics/Graphics.hpp"
#include "LuxEngine/Types/LuxBool.hpp"
#include "vulkan/vulkan.h"
//#include "LuxEngine/Core/ConsoleOutput.h"
#define LUX_H_CORE







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








//TODO variable initialization depends on link order
namespace lux::core{
	extern double	FPS;
	extern float	FOV;
	extern bool		running;
	extern bool		useVSync;
	extern bool		initialized;
	extern uint32	frames;
	//extern bool postInitialized = false;

	extern VkInstance					instance;
	extern VkDebugUtilsMessengerEXT		debugMessenger;
	extern VkSurfaceKHR					surface;

	extern const char**						validationLayers;
	extern const char**						requiredDeviceExtensions;
	extern uint32						validationLayersNum;
	extern uint32						requiredDeviceExtensionsNum;
	//extern DynArray<const String>	validationLayers;
	//extern DynArray<const String>	requiredDeviceExtensions;



	//Initialization
	#pragma optimize("", off)
	void preInit_( );
	void preInit( );
	void init(bool useVSync);
	// struct PreInitializer{ PreInitializer( ){ lux::core::preInit( ); } };
	// struct PostInitializer{ PostInitializer( ){ lux::core::init(false); } };
	// //TODO this probably depends on the link order
	// extern PreInitializer luxPreInitializer; //This variable is used to inizialize the engine before any other variable or function call
	#define PostInitializer(fileName)										\
		lux::core::PreInitializer luxGeneratedPreInitializer_##fileName;	\
		lux::core::PostInitializer luxGeneratedPostInitializer_##fileName
	#pragma optimize("", on)

	//Main
	void run(bool vUseVSync, float vFOV);
	void mainLoop( );
	void runFPSCounterThr( );
	void runRenderThr( );




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