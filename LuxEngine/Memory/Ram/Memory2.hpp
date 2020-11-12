#ifndef LUX_H_MEMORY_2
#define LUX_H_MEMORY_2


#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/LuxBool.hpp"
#include "LuxEngine/Memory/Memory_t.hpp"
#ifdef _WIN64
#include <intrin.h>
#elif defined __linux__
#include <x86intrin.h>
#endif


// #pragma warning( disable : 4227 )    //"Anachronism used: qualifiers on reference are ignored"
namespace lux::ram{
	#define LuxMemOffset 32

	void cpy(const void* const src, void* const dst, uint64 num, const LuxBool thr = LUX_AUTO);
	template<class t> static inline void cpy(const ram::ptr<const t>& src, const ram::ptr<t>& dst, uint64 num, const LuxBool thr = LUX_AUTO){
		cpy(src.cell->address, dst.cell->address, num, thr);
	}
	void cpy_thr(const __m256i* src, __m256i* dst, uint64 num);
};
// #pragma warning( default : 4227 )
#endif