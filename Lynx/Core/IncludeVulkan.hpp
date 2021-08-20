#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"	//Turn off variable shadowing warning. Vulkan really likes to shadow its variables
#include "Lynx/Core/VkDef.hpp"				//Include Vulkan macros for intellisense to work properly. This file is included automatically during compilation
//FIXME^ include from intellisense includes
//FIXME^ or don't. idk. other parsers may not work, that way
#include <vulkan/vulkan.hpp>				//Include C++ Vulkan
#pragma GCC diagnostic pop

#include "Lynx/macros.hpp"					//Include macros header for _dbg, _rls, and noop macros








/**
 * @brief Manages out of memory and unknown results
 */
#define vkDefaultFaulures													\
	/*No vram available. Use ram*/											\
	case vk::Result::eErrorOutOfDeviceMemory: { 							\
		dbg::logError("Out of device memory");							\
		break;																\
	}																		\
\
	/*No ram or swap available. Pack pool cells and free unused blocks*/	\
	case vk::Result::eErrorOutOfHostMemory: {								\
		dbg::logError("Out of host memory");								\
		break;																\
	}																		\
\
	/*If in debug mode, catch any result that was not checked*/				\
	default: {																\
		_dbg(dbg::logError("Unknown result"))_rls(break);					\
	}








/**
 * @brief Manages failure cases and vkSuccess
 */
#define vkDefaultCases							\
	/*Exit switch if the call succeeded*/		\
	case vk::Result::eSuccess: break;			\
\
	/*Manage failure cases*/					\
	vkDefaultFaulures
