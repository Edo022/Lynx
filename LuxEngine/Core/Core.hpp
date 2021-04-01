#pragma once
#define LUX_H_CORE
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/Render/Render.hpp"
#include "LuxEngine/Threads/Thread.hpp"






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








namespace lux::core{
	extern std::atomic<bool> running;
	extern Thread inputThr;


	void inputLoop();




	namespace debug {
		//It's dark magic, idk why or how it works, but it does
		inline VkResult CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger) {
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
			if(func != nullptr) return func(
				instance, //FIXME CHECK IF THOSE TYPES ARE BINARY COMPATIBLE
				reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(pCreateInfo),
				reinterpret_cast<const VkAllocationCallbacks*>(pAllocator),
				reinterpret_cast<VkDebugUtilsMessengerEXT*>(pDebugMessenger)
			);
			else return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
		inline void DestroyDebugUtilsMessengerEXT(vk::Instance vInstance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vInstance, "vkDestroyDebugUtilsMessengerEXT");
			if(func != nullptr) func(vInstance, debugMessenger, reinterpret_cast<const VkAllocationCallbacks*>(pAllocator));
		}


		//More dark magic
		static inline void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
			createInfo = vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity (vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError  )
				.setMessageType     (vk::DebugUtilsMessageTypeFlagBitsEXT    ::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT:: eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
			;
			_dbg(createInfo.setPfnUserCallback(core::render::vulkanOutputCallback));
		}
	}
}
