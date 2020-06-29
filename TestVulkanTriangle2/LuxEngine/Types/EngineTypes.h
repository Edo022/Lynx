#pragma once
#include "LuxEngine/Engine/Devices_t.h"
#include "LuxEngine/macros.h"
#include "Integers/Integers.h"  // for uint32, uint64
#include "vulkan/vulkan_core.h" // for VkPhysicalDeviceLimits, VkPhysicalDeviceProperties




typedef uint64 LuxShader;
typedef uint64 LuxCell, LuxBuffer;




//#LLID ECF0000
//Returns a LuxCell created from is buffer index, cell size, cell index and shared properties
// .            |    shared    max          shift    |    dedicated    max             shift
// isShared:    |    1b        0            << 63    |    1b           1               << 63
// buffer:      |    11b       2 048        << 52    |    11b          2 048           << 52
// cellIndex:   |    26b       67 108 864   << 26    |    -            -               -    
// cellSize:    |    26b       67 108 864   -        |    32b          4 294 967 296   -    
static constexpr LuxCell __lp_cellCode(bool vIsShared, uint32 vBuffer, uint32 vCellIndex, uint64 vCellSize) {
	//uint64 code = vCellSize;
	//if (vIsShared) code |= ((scast<uint64>(vCellIndex) & 0x3FFffff) << 26);
	//return code | ((scast<uint64>(vBuffer) & 0xFFFFffff) << 52) | (scast<uint64>(vIsShared) << 63);
	if (vIsShared) vCellSize |= ((scast<uint64>(vCellIndex) & 0x3FFffff) << 26);
	return vCellSize | ((scast<uint64>(vBuffer) & 0xFFFFffff) << 52) | (scast<uint64>(vIsShared) << 63);
}


static constexpr bool   __lp_isShared_from_cc(LuxCell vCell) { return vCell >> 63; }
static constexpr uint32 __lp_buffer_from_cc(LuxCell vCell) { return ((vCell) >> 52) & 0x7FF; }
static constexpr uint32 __lp_cellIndex_from_cc(LuxCell vCell) { return (__lp_isShared_from_cc(vCell)) ? (((vCell) >> 26) & 0x3FFffff) : (0); }
static constexpr uint32 __lp_cellSize_from_cc(LuxCell vCell) { return (vCell) & 0x3FFffff; }

//Returns the offset of a LuxCell calculated with its cell code and physical device
static constexpr uint32 __lp_cellOffset_from_cc(const _VkPhysicalDevice* pDevice, const LuxCell vCell) {
	const uint32 rawOffset = (__lp_cellIndex_from_cc(vCell) * __lp_cellSize_from_cc(vCell));
	if (__lp_isShared_from_cc(vCell) == 0 || rawOffset == 0) return 0;
	else return scast<uint32>(rawOffset - (rawOffset % pDevice->properties.limits.minStorageBufferOffsetAlignment) + pDevice->properties.limits.minStorageBufferOffsetAlignment);
}
