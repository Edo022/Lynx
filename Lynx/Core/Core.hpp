#pragma once
#define LNX_H_CORE
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Lynx/Threads/Thread.hpp"






/*
Object rendering
.																							   Render space assembler
.																										¦
.																										¦
.																										¦
.																			  Render space assembler    ¦
.																						 ¦              ¦
.																						 ¦              ¦     Render space assembler
.																						 ¦              ¦              ¦
.																	SortByZIndex()       ¦              ¦              ¦
.								  Objects cache                     ToGlobalPos()        ¦              ¦              ¦
.								  .───────────────────.                                  ¦              ¦          .───¦───────────────.
.								   \     .─────────.   \                                 ¦              ¦           \  ¦  .─────────.   \
.		  Object 0 --> Draw() ----> \     \_________\   \ ----------> --------------> ---¦ ------------------------> \     \_________\   \
.		  Ozindex = 2                \   Layer 0         \                               ¦                            \    Layer 0        \
.								  .───\                   \                              ¦              ¦  .───────────\                   \
.								   \   \___________________\                             ¦              ¦   \          ¦\___________________\
.		  Object 1 --> Draw() ----> \                    \ ---------> ---.     .----> ---¦ ----------------> \   Layer ¦ 2  \____\
.		  zindex = 0              .──\ Layer 1            \               '. .'          ¦                    \        ¦          \
.								   \  \____________________\                X            ¦              ¦      \___________________\───.
.									\  Layer 2     \____\                 .' '.          ¦              ¦          \   ¦                \
.		  Object 2 --> Draw() -----> \                   \ ---------> ---'     '----> ---¦ -----------------------> \                    \
.		  zindex = 1                  \___________________\                              ¦                           \ Layer 1            \
.																						 ¦              ¦             \____________________\
.																						 ¦              ¦              ¦
.																						 ¦              ¦              ¦
.																						 ¦              ¦              ¦
.	i input                                                                              ¦              ¦              ¦
.	u update                                                                             ¦              ¦              ¦
.	l draw loop                                                                          ¦              ¦              ¦
.	r read, w write                                                                      ¦              ¦              ¦
.																						 ¦              ¦              ¦
.   2D object                                                                            ¦    .         ↓              ¦              .
.	   i, u | in object data | w SHARED                                                  ¦     \ Render space 2        ¦               \
.	   i, u | Render object  | r SHARED, w VRAM                                      _ __¦ _    \______________________¦ _______________\                                            Window output                                 RS2
.	   l    | Display cache  | w VRAM                                                    ¦  \                          ¦                                                              ___________________________________________________________________
.																						 ↓   \     Render space 0      ¦                                                             │            ___________    │ /                                 │   │
.   3D object                                                                       Render    \    ____________________¦ _____________________                                       │           │           │   │___________________________________│   │
.	   i, u | in object data | w SHARED --> VRAM                                     space 1   \   \                   ↓                      \                                      │   _____   │ /         │                                           │
.	   l    | Render object  | w VRAM                                                           \   \      .─ ─ ─ ─ ─ ─ ─ ─ ─ ─.               \                                     │  │     │  │ /         │    ____________________________________   │
.																								 \   \      \      .─ ─ ─ ─ ─ ─ ─ ─ ─ ─.        \                                    │  │ /   │  │ /         │   │                                    │  │
.																								  \   \      \      \     .─ ─ ─ ─ ─.   \        \  ------> CopyBuffer() --------->  │  │ /   │  │ /         │   │     .───────────────────.          │  │
.																								   \   \      \      \     \_ _ _ _ _\   \        \                                  │  │ /   │  │ /         │   │     │      .───────────────────.   │  │
.																									\   \      \ _ _ _\                   \        \                                 │  │ /   │  │ /         │   │     │   Lay│     .─────────.   │   │  │
.																									 \   \          \  \                   \        \                                │  │ /   │  │ /         │   │     │      │     │_________│   │   │  │
.																									  \   \          \  \_ _ _ _ _ _ _ _ _ _\        \                               │  │ /   │  │ /         │   │     │ _____│    Layer 0        │   │  │
.																									   \   \          \ _ _ _ _ _ _ _ _ _ _\          \                              │  │ /   │  │           │   │         │  │                   │   │  │
.																										\   \                                          \                             │  │ /   │  │           │   │         │ L│___________________│   │  │
.																										 \   \                                          \                            │  │     │  │           │   │         │____________________│     │  │
.																									_ ____\   \__________________________________________\                           │  │     │  │           │   │                                    │  │
.																																													 │  │_____│  │___________│   │____________________________________│  │
.																																													 │___________________________________________________________________│
.																																														  RS3          RS1                         RS0
*/








namespace lnx::core{
	extern std::atomic<bool> running;
	extern Thread inputThr;


	void inputLoop();




	namespace debug {
		//It's dark magic, idk why or how it works, but it does
		inline VkResult CreateDebugUtilsMessengerEXT(vk::Instance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
			if(func != nullptr) return func(
				instance, //! Ok. vk::Instance has implicit conversion to VkInstance
				pCreateInfo,
				pAllocator,
				pDebugMessenger
			);
			else return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
		inline void DestroyDebugUtilsMessengerEXT(vk::Instance vInstance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vInstance, "vkDestroyDebugUtilsMessengerEXT");
			if(func != nullptr) func(vInstance, debugMessenger, pAllocator);
		}


		//More dark magic
		static inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
			createInfo = VkDebugUtilsMessengerCreateInfoEXT{
				.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				.pfnUserCallback = core::render::vulkanOutputCallback
			};
		}
	}
}
