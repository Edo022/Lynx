
#include "LuxEngine/Memory/Memory.h"
#include "LuxEngine/Threads/ThreadPool.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/macros.h"


//11GB copy (10 * 1,100,100,100 B)

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
//cpy __m256i + --l 1l const 2 fdmv	1.698120	1.712640	1.722231
//cpy // + unwr16					-			-			-




//41GB copy (10 * 4,100,100,100 B)

//memcpy AVX2D						7.360948	7.122496
//cpy unwr64 + sw64					6.323193	6.338815
//cpy unwr64 + sw64 + 2thr			3.799257	3.882980	4.073120
//cpy unwr64 + sw64 + 4thr



//TODO add [no AVX2] performance warning
namespace lux::mem{
	//memcpy, but faster (~1.2 times faster in single thread,
	//The function will not return until all threads have completed the operation
	//Use lux::mem::async::cpy to asynchronously copy data in a buffer
	//Source and destination buffers should not overlap. If they do, it's undefined behaviour
	//*   src | address of the source buffer
	//*   dst | address of the destination buffer
	//*   num | number of bytes to copy
	//*   thr | number of threads to use. Default: LUX_AUTO
	//*      Multithreading could not be used in operations with small buffers, as it would negatively affect performance
	void cpy(const void* const src, void* const dst, uint64 num, const int32 thr){
		switch(thr){
			case LUX_AUTO:
			case 1: cpy_thr((__m256i*)src, (__m256i*)dst, num); break;
			case 2: {
				uint64 numShift = multipleOf(num / 2, 32);
				bool thrf = false;
				lux::thr::sendToExecQueue(cpy_thr, lux::thr::Priority::LUX_PRIORITY_MAX, &thrf, (__m256i*)src, (__m256i*)dst, numShift);
				cpy_thr((const __m256i*)((const uint64)src + numShift), (__m256i*)((uint64)dst + numShift), num - numShift);
				while(!thrf) sleep(5);
				break;
			}
			default: core::printError("Invalid number of threads passed to memcpy function", thr, true);
		}
	}




	void cpy_thr(const __m256i* src, __m256i* dst, uint64 num){
		if((uint64)src % 32 != 0 || (uint64)dst % 32 != 0 || num % 32 != 0) {
			memcpy(dst, src, num);
			return;
		}

		//Copy 2048 bytes per iteration
		//const __m256i* src256 = rcast<const __m256i*>(src);
		//__m256i* dst256 = rcast<__m256i*>(dst);
		#define iter _mm256_stream_si256(dst, _mm256_stream_load_si256(src)); ++src; ++dst;
		//#define iter _mm256_stream_si256(dst256, _mm256_stream_load_si256(src256)); ++src256; ++dst256;
		#define iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter;
		for(uint64 a = (num = (num / 32 + !!(num % 32))) / 64; a; --a) { iter16 iter16 iter16 iter16 }

		//Copy all the remaining bytes
		//sorry UwU
		switch(num % 64){
			case 1:  { iter return; }
			case 2:  { iter iter return; }
			case 3:  { iter iter iter return; }
			case 4:  { iter iter iter iter return; }
			case 5:  { iter iter iter iter iter return; }
			case 6:  { iter iter iter iter iter iter return; }
			case 7:  { iter iter iter iter iter iter iter return; }
			case 8:  { iter iter iter iter iter iter iter iter return; }
			case 9:  { iter iter iter iter iter iter iter iter iter return; }
			case 10: { iter iter iter iter iter iter iter iter iter iter return; }
			case 11: { iter iter iter iter iter iter iter iter iter iter iter return; }
			case 12: { iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 13: { iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 14: { iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 15: { iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 16: { iter16 return; }
			case 17: { iter16 iter return; }
			case 18: { iter16 iter iter return; }
			case 19: { iter16 iter iter iter return; }
			case 20: { iter16 iter iter iter iter return; }
			case 21: { iter16 iter iter iter iter iter return; }
			case 22: { iter16 iter iter iter iter iter iter return; }
			case 23: { iter16 iter iter iter iter iter iter iter return; }
			case 24: { iter16 iter iter iter iter iter iter iter iter return; }
			case 25: { iter16 iter iter iter iter iter iter iter iter iter return; }
			case 26: { iter16 iter iter iter iter iter iter iter iter iter iter return; }
			case 27: { iter16 iter iter iter iter iter iter iter iter iter iter iter return; }
			case 28: { iter16 iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 29: { iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 30: { iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 31: { iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 32: { iter16 iter16 return; }
			case 33: { iter16 iter16 iter return; }
			case 34: { iter16 iter16 iter iter return; }
			case 35: { iter16 iter16 iter iter iter return; }
			case 36: { iter16 iter16 iter iter iter iter return; }
			case 37: { iter16 iter16 iter iter iter iter iter return; }
			case 38: { iter16 iter16 iter iter iter iter iter iter return; }
			case 39: { iter16 iter16 iter iter iter iter iter iter iter return; }
			case 40: { iter16 iter16 iter iter iter iter iter iter iter iter return; }
			case 41: { iter16 iter16 iter iter iter iter iter iter iter iter iter return; }
			case 42: { iter16 iter16 iter iter iter iter iter iter iter iter iter iter return; }
			case 43: { iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter return; }
			case 44: { iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 45: { iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 46: { iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 47: { iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 48: { iter16 iter16 iter16 return; }
			case 49: { iter16 iter16 iter16 iter return; }
			case 50: { iter16 iter16 iter16 iter iter return; }
			case 51: { iter16 iter16 iter16 iter iter iter return; }
			case 52: { iter16 iter16 iter16 iter iter iter iter return; }
			case 53: { iter16 iter16 iter16 iter iter iter iter iter return; }
			case 54: { iter16 iter16 iter16 iter iter iter iter iter iter return; }
			case 55: { iter16 iter16 iter16 iter iter iter iter iter iter iter return; }
			case 56: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter return; }
			case 57: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter return; }
			case 58: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter return; }
			case 59: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter return; }
			case 60: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 61: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 62: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 63: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			case 64: { iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; }
			default: return;
		}
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