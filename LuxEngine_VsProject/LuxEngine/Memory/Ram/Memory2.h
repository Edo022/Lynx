#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/LuxBool.h"
#include "LuxEngine/Memory/Memory_t.h"
#include <intrin.h>


#pragma warning( disable : 4227 )    //"Anachronism used: qualifiers on reference are ignored"
namespace lux::mem{
	void cpy(const void* const src, void* const dst, uint64 num, const LuxBool thr = LUX_AUTO);
	template<class t> static inline void cpy(const ram::ptr<t>& const src, ram::ptr<t>& const dst, uint64 num, const LuxBool thr = LUX_AUTO){ cpy(src.cell->address, dst.cell->address, num, thr); }
	void cpy_thr(const __m256i* src, __m256i* dst, uint64 num);
};
#pragma warning( default : 4227 )
