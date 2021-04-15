#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"	//Turn off variable shadowing warning. Vulkan really likes to shadow its variables
#include "Lynx/Core/VkDef.hpp"				//Include Vulkan macros for intellisense to work properly
#include <vulkan/vulkan.hpp>				//Include C++ Vulkan
#pragma GCC diagnostic pop

#include "Lynx/macros.hpp"					//Include macros header for _dbg, _rld and noop macros




#define vkDefaultFaulures					/*Manage out of memory and unknown results*/								\
	case vk::Result::eErrorOutOfDeviceMemory: 	/*No vram available. Use ram*/											\
		dbg::printError("Out of device memory"); break;																	\
	case vk::Result::eErrorOutOfHostMemory:		/*No ram or swap available. Pack pool cells and free unused blocks*/	\
		dbg::printError("Out of host memory");   break;																	\
	default: 									/*In debug mode, catch any result that was not checked*/            	\
		_dbg(dbg::printError("Unknown result")) _rls(noop)


#define vkDefaultCases						/*Manage failure cases and vkSuccess*/										\
	case vk::Result::eSuccess: break; 			/*Exit switch if the call succeeded*/									\
	vkDefaultFaulures							/*Copy failure cases*/
