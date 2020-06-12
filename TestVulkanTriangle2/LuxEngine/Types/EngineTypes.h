#pragma once
#include "Integers/Integers.h"


typedef uint64 LuxShader;
typedef uint64 LuxCell;


// isShared: 1b
// buffer: 12b
// fragIndex: 20b
// fragSize: 31b
#define luxFragCode(buffer, fragIndex, fragSize) (((uint64)1 << 63) | ((uint64)buffer << 51) | ((uint64)fragIndex << 31) | ((uint64)fragSize & 0x7FFFfFFF))
#define luxIsSharedFromFragCode(fragCode) ((fragCode >> 63) & 0b1)
#define luxBufferFromFragCode(fragCode) ((fragCode >> 51) & 0xFFF)
#define luxFragIndexFromFragCode(fragCode) ((fragCode >> 31) & 0xfFFFF)
#define luxFragSizeFromFragCode(fragCode) (fragCode & 0x7FFFffff)