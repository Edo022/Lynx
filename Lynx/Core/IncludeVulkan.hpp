#include "Lynx/macros.hpp"
#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <vulkan/vulkan.hpp>
#pragma GCC diagnostic pop


#define vkDefaultCases                                                                        \
	case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break; \
	case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of device memory"); break; \
	default: _dbg(dbg::printError("Unknown result")) _rls(noop)

#define vkDefaultCases2               \
	case vk::Result::eSuccess: break; \
	vkDefaultCases
