
#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Core.h"
#include <intrin.h>


namespace lux::mem{
	void cpy(const void* const src, void* const dst, uint64 num, const LuxBool thr = LUX_AUTO);
	void cpy_thr(const __m256i* src, __m256i* dst, uint64 num);
}
