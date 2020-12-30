#include "LuxEngine/Core/Memory/Ram/Ram.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"

// #include "LuxEngine/Types/Pointer.hpp"
#include "LuxEngine/Core/Memory/Shared.hpp"

#include <cstring>
//TODO background cell preallocation
//TODO add [no AVX2] performance warning
//TODO add AVX512 when supported //or don't. it's probably useless








//TODO choose number of buffers based on the system memory
namespace lux::ram{
	Type_t types[(uint32)lux::__pvt::CellClassIndex::NUM];
	uint32 allocated;

	RaArrayC<Cell_t> cells;



	luxAutoInit(LUX_H_MEMORY){
		//Initialize buffer types. Allocate enough cells and buffers to use the whole RAM
		for(uint32 i = 0; i < (uint32)lux::__pvt::CellClassIndex::NUM; ++i){
			uint32 cellsNum = systemMemory / (uint64)lux::__pvt::classEnumFromIndex(i);
			uint32 buffsNum = systemMemory / lux::__pvt::bufferSize;
			types[i] = {
				.cellClass = lux::__pvt::classEnumFromIndex(i),
				.memory =  (void** )calloc(sizeof(void*), buffsNum),		//Max number of buffers. Initialize them with nullptr
				.cellsPerBuff = cellsNum / buffsNum
			};
			types[i].cells.init(cellsNum);
		}
		cells.init(systemMemory / (uint64)lux::CellClass::CLASS_A);		//Preallocate the max number of cells of the class with the min size
	}








	//memcpy, but faster. The performance difference is much more noticeable in dual / quad channel systems, or systems with high frequency RAM / low frequency CPU
	//The function will not return until all threads have completed the operation
	//Use lux::mem::acpy to asynchronously copy data in a buffer
	//Source and destination buffers should not overlap. If they do, it's undefined behaviour
	//*  src | address of the source buffer
	//*  dst | address of the destination buffer
	//*  num | number of bytes to copy
	//*  thr | LUX_TRUE to use multithreading, LUX_FALSE to use 1 thread. Default: LUX_AUTO
	//*   Multithreading cannot be used in operations with small buffers, as it would negatively affect performance
	void cpy(const void* const src, void* const dst, uint64 num/*, const LuxBool thr*/){
		//luxDebug(if((uint64)src % 32 != 0)	param_error(src, "Misaligned address. This function should only be used with aligned addresses and count. Use ucpy to copy unaligned data (this will negatively affect performance)"));
		//luxDebug(if((uint64)dst % 32 != 0)	param_error(dst, "Misaligned address. This function should only be used with aligned addresses and count. Use ucpy to copy unaligned data (this will negatively affect performance)"));
		//luxDebug(if(num % 32 != 0)			param_error(num, "Misaligned count. This function should only be used with aligned addresses and count. Use ucpy to copy unaligned data (this will negatively affect performance)"));

		// switch(thr){
			// case LUX_AUTO: if(num > 32 * 64 * 128) goto __2thrCase; [[fallthrough]];
			// case LUX_FALSE: cpy_thr((__m256i*)src, (__m256i*)dst, num); break;
			// case LUX_TRUE: { __2thrCase:
			// 	uint64 numShift = multipleOf(num / 2, LuxMemOffset); bool thrf = false;
			// 	lux::thr::sendToExecQueue(cpy_thr, thr::Priority::LUX_PRIORITY_MAX, &thrf, (__m256i*)src, (__m256i*)dst, numShift);
			// 	cpy_thr((const __m256i*)((const uint64)src + numShift), (__m256i*)((uint64)dst + numShift), num - numShift);
			// 	while(!thrf) sleep(5); break;
			// }
			// default: param_error(thr, "Valid values: LUX_TRUE, LUX_FALSE, LUX_AUTO");
		// }

		// // cpy_thr((__m256i*)src, (__m256i*)dst, num);
		memcpy(dst, src, num);
	}



	//TODO implement correct cpy_thr
	void cpy_thr(const __m256i* src, __m256i* dst, uint64 num){
		// // // // memcpy(dst, src, num);
		////Copy bytes with index >= 2048
		//#define iter _mm256_stream_si256(dst++, _mm256_stream_load_si256(src++));
		//#define iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter;
		////for(uint64 a = (num = (num / 32 + !!(num % 32))) / 64; a; --a) { iter16 iter16 iter16 iter16 }
		//for(uint64 a = (num = (num / 32 + !!(num % 32))) / 64; a; --a) { iter16 iter16 iter16 iter16 }

		////Copy all the remaining bytes //sorry UwU
		//switch(num % 64){
		//	case 0: return; case 1: iter return; case 2: iter iter return; case 3: iter iter iter return; case 4: iter iter iter iter return; case 5: iter iter iter iter iter return; case 6: iter iter iter iter iter iter return; case 7: iter iter iter iter iter iter iter return; case 8: iter iter iter iter iter iter iter iter return; case 9: iter iter iter iter iter iter iter iter iter return; case 10: iter iter iter iter iter iter iter iter iter iter return; case 11: iter iter iter iter iter iter iter iter iter iter iter return; case 12: iter iter iter iter iter iter iter iter iter iter iter iter return; case 13: iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 14: iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 15: iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return;
		//	case 16: iter16 return; case 17: iter16 iter return; case 18: iter16 iter iter return; case 19: iter16 iter iter iter return; case 20: iter16 iter iter iter iter return; case 21: iter16 iter iter iter iter iter return; case 22: iter16 iter iter iter iter iter iter return; case 23: iter16 iter iter iter iter iter iter iter return; case 24: iter16 iter iter iter iter iter iter iter iter return; case 25: iter16 iter iter iter iter iter iter iter iter iter return; case 26: iter16 iter iter iter iter iter iter iter iter iter iter return; case 27: iter16 iter iter iter iter iter iter iter iter iter iter iter return; case 28: iter16 iter iter iter iter iter iter iter iter iter iter iter iter return; case 29: iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 30: iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 31: iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return;
		//	case 32: iter16 iter16 return; case 33: iter16 iter16 iter return; case 34: iter16 iter16 iter iter return; case 35: iter16 iter16 iter iter iter return; case 36: iter16 iter16 iter iter iter iter return; case 37: iter16 iter16 iter iter iter iter iter return; case 38: iter16 iter16 iter iter iter iter iter iter return; case 39: iter16 iter16 iter iter iter iter iter iter iter return; case 40: iter16 iter16 iter iter iter iter iter iter iter iter return; case 41: iter16 iter16 iter iter iter iter iter iter iter iter iter return; case 42: iter16 iter16 iter iter iter iter iter iter iter iter iter iter return; case 43: iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter return; case 44: iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter return; case 45: iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 46: iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 47: iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return;
		//	case 48: iter16 iter16 iter16 return; case 49: iter16 iter16 iter16 iter return; case 50: iter16 iter16 iter16 iter iter return; case 51: iter16 iter16 iter16 iter iter iter return; case 52: iter16 iter16 iter16 iter iter iter iter return; case 53: iter16 iter16 iter16 iter iter iter iter iter return; case 54: iter16 iter16 iter16 iter iter iter iter iter iter return; case 55: iter16 iter16 iter16 iter iter iter iter iter iter iter return; case 56: iter16 iter16 iter16 iter iter iter iter iter iter iter iter return; case 57: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter return; case 58: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter return; case 59: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter return; case 60: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter return; case 61: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 62: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter return; case 63: iter16 iter16 iter16 iter iter iter iter iter iter iter iter iter iter iter iter iter iter iter return;
		//}
		//_mm_sfence( );
	}
}

