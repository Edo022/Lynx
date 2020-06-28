#pragma once
#include "LuxEngine/Engine/Devices.struct.h"
#include "LuxEngine/macros.h"
#include "Integers/Integers.h"  // for uint32, uint64
#include "vulkan/vulkan_core.h" // for VkPhysicalDeviceLimits, VkPhysicalDeviceProperties



typedef uint64 LuxShader;
typedef uint64 LuxCell, LuxBuffer;

// isShared: 1b
// buffer: 12b
// cellIndex: 20b
// cellSize: 31b
#define __lp_cellCode(isShared, buffer, cellIndex, cellSize) (((uint64)isShared << 63) | ((uint64)(buffer) << 51) | ((uint64)(cellIndex) << 31) | ((cellSize) & 0x7FFFfFFF))
#define __lp_isShared_from_cc(cellCode) (((cellCode) >> 63) & 0b1)
#define __lp_buffer_from_cc(cellCode) (((cellCode) >> 51) & 0xFFF)
#define __lp_cellIndex_from_cc(cellCode) (((cellCode) >> 31) & 0xfFFFF)
#define __lp_cellSize_from_cc(cellCode) ((cellCode) & 0x7FFFffff)

constexpr uint32 __lp_cellOffset_from_cc(const _VkPhysicalDevice* pDevice, const LuxCell vCell) {
	const uint32 rawOffset = (__lp_cellIndex_from_cc(vCell) * __lp_cellSize_from_cc(vCell));
	if (__lp_isShared_from_cc(vCell) == 0 || rawOffset == 0) return 0;
	else return scast<uint32>(rawOffset - (rawOffset % pDevice->properties.limits.minStorageBufferOffsetAlignment) + pDevice->properties.limits.minStorageBufferOffsetAlignment);
}
