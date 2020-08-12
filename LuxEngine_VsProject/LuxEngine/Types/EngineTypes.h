#pragma once
#include "LuxEngine/Core/Devices_t.h"
#include "LuxEngine/macros.h"
#include "Integers/Integers.h"  // for uint32, uint64
#include "vulkan/vulkan_core.h" // for VkPhysicalDeviceLimits, VkPhysicalDeviceProperties




typedef uint64 LuxShader, LuxBuffer;
typedef uint64 LuxCell;




//#LLID ECF0000
//Returns a LuxCell created from is buffer index, cell size, cell index and shared properties
// .            |    fixed     max          shift    |    customSize   max             shift
// cellType:    |    1b        0            << 63    |    1b           1               << 63
// buffer:      |    11b       2 048        << 52    |    11b          2 048           << 52
// cellIndex:   |    26b       67 108 864   << 26    |    -            -               -
// cellSize:    |    26b       67 108 864   -        |    32b          4 294 967 296   -
static constexpr LuxCell createLuxCell(bool vIsShared, uint32 vBuffer, uint32 vCellIndex, uint64 vCellSize) {
	if (vIsShared) vCellSize |= ((scast<uint64>(vCellIndex) & 0x3FFffff) << 26);
	return vCellSize | ((scast<uint64>(vBuffer) & 0xFFFFffff) << 52) | (scast<uint64>(vIsShared) << 63);
}


static constexpr bool   getCellType(LuxCell vCell) { return vCell >> 63; }													//Returns true if the cell is in a fixed size buffer, false if it's a custom size one
static constexpr uint32 getBufferIndex(LuxCell vCell) { return ((vCell) >> 52) & 0x7FF; }									//Returns the index of the buffer of a LuxCell
static constexpr uint32 getCellIndex(LuxCell vCell) { return (getCellType(vCell)) ? (((vCell) >> 26) & 0x3FFffff) : (0); }	//Returns the index of a LuxShader in its buffer
static constexpr uint32 getCellSize(LuxCell vCell) { return (vCell) & 0x3FFffff; }											//Returns the size of a LuxShader in bytes
static constexpr uint32 getCellOffset(const _VkPhysicalDevice* pDevice, const LuxCell vCell) {								//Returns the offset of a LuxCell
	const uint32 rawOffset = (getCellIndex(vCell) * getCellSize(vCell));
	if (getCellType(vCell) == 0 || rawOffset == 0) return 0;
	//else return scast<uint32>(rawOffset - (rawOffset % pDevice->properties.limits.minStorageBufferOffsetAlignment) + pDevice->properties.limits.minStorageBufferOffsetAlignment);
	else return rawOffset;
}
//static constexpr uint32 getCellOffsetUniform(const _VkPhysicalDevice* pDevice, const LuxCell vCell) {								//Returns the offset of a LuxCell
//	const uint32 rawOffset = (getCellIndex(vCell) * getCellSize(vCell));
//	if (getCellType(vCell) == 0 || rawOffset == 0) return 0;
//	else return scast<uint32>(rawOffset - (rawOffset % pDevice->properties.limits.minUniformBufferOffsetAlignment) + pDevice->properties.limits.minUniformBufferOffsetAlignment);
//}
static constexpr uint32 getCellOffsetUniform(const _VkPhysicalDevice* pDevice, const LuxCell vCell) {								//Returns the offset of a LuxCell
	return getCellOffset(pDevice, vCell);
}


