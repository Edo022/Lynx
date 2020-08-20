#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/LuxBool.h"
#include "LuxEngine/Memory/Memory_t.h"
#include <intrin.h>


namespace lux::mem{
	void cpy(const void* const src, void* const dst, uint64 num, const LuxBool thr = LUX_AUTO);
	static inline void cpy(const ram::Cell_t* const src, ram::Cell_t* const dst, uint64 num, const LuxBool thr = LUX_AUTO){ cpy(src->address, dst->address, num, thr); }
	void cpy_thr(const __m256i* src, __m256i* dst, uint64 num);
};
