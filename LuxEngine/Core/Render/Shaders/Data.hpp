#pragma once
#include <vulkan/vulkan.h>

namespace lux::core::c::shaders{
    extern VkImageMemoryBarrier readToWriteBarrier;
    extern VkImageMemoryBarrier writeToReadBarrier;
    extern VkBufferImageCopy copyRegion;
}