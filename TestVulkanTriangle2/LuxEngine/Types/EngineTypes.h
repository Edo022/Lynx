#pragma once
#include "Integers/Integers.h"


typedef uint64 LuxShader;
typedef uint64 LuxCell;


// isShared: 1b
// buffer: 12b
// cellIndex: 20b
// cellSize: 31b
//Cell size 0 means that the buffer is not shared
#define __lp_cellCode(buffer, cellIndex, cellSize) (((cellSize == 0) ? ((uint64)1 << 63) : 0) | ((uint64)buffer << 51) | ((uint64)cellIndex << 31) | (cellSize & 0x7FFFfFFF))
#define __lp_isShared_from_cc(cellCode) ((cellCode >> 63) & 0b1)
#define __lp_buffer_from_cc(cellCode) ((cellCode >> 51) & 0xFFF)
#define __lp_cellIndex_from_cc(cellCode) ((cellCode >> 31) & 0xfFFFF)
#define __lp_cellSize_from_cc(cellCode) (cellCode & 0x7FFFffff)