//This is the first file to be included during compilation
//It contains Vulkan macros that are redefined by the engine
#pragma once

#define GLM_FORCE_RADIANS                   //Define vulkan macros
#define GLM_FORCE_DEPTH_ZERO_TO_ONE         //Define vulkan macros
#define VULKAN_HPP_NO_EXCEPTIONS            //No Vulkan exceptions. Results are checked manually
#define VULKAN_HPP_ASSERT(...) void(0)      //No Vulkan exceptions. Results are checked manually
