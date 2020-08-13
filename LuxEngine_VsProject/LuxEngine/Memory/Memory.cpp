
#include "LuxEngine/Memory/Memory.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/macros.h"
#include <intrin.h>

#include <assert.h>

//11GB copy (1,100,100,100 B)

//memcpy							1.995105	2.034764	2.020125
//memcpy AVX2D						2.072510	1.972521	1.973882

//cpy								?			-			-
//cpy AVXD							~120		-			-
//cpy wr16   + l16 AVX2D			6.808085	6.786989	6.800637
//cpy wr16   + l16 AVX2D			3.916548	3.890693	3.894911
//cpy wr16-- + l16 AVX2D			3.866092	3.942484	3.874485
//cpy wr16-- + sw16 AVX2D			3.900137	3.900659	3.889693
//cpy __m256i + l-- r				1.761605	1.685134	1.707356
//cpy __m256i + --l 1l const -j		1.710174	1.712255	1.766191
//cpy __m256i + --l 1l const 2		1.718665	-			-
//cpy __m256i + --l 1l const 2 fdmv




namespace lux::mem{
	//Like memcpy, but faster
	//Way faster than standard memcpy
	//Source and destination should not overlap
	void cpy(const void* src, void* dst, uint64 num){
		const __m256i* src256 = rcast<const __m256i*>(src);
		__m256i* dst256 = rcast<__m256i*>(dst);
		for(uint64 i = num / 32 + !!(num % 32); i; --i, ++src256, ++dst256) _mm256_stream_si256(dst256, _mm256_stream_load_si256(src256));
		_mm_sfence( );




		//assert(num % 32 == 0);
		//assert((intptr_t(dst) & 31) == 0);
		//assert((intptr_t(src) & 31) == 0);
		//const __m256i* pSrc = reinterpret_cast<const __m256i*>(src);
		//__m256i* pDest = reinterpret_cast<__m256i*>(dst);
		//int64 nVects = num / sizeof(*pSrc);
		//for(; nVects > 0; nVects--, pSrc++, pDest++) {
		//	const __m256i loaded = _mm256_stream_load_si256(pSrc);
		//	_mm256_stream_si256(pDest, loaded);
		//}
		//_mm_sfence( );

	}
}