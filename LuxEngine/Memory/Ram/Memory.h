#ifndef LUX_H_MEMORY
#define LUX_H_MEMORY


#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Memory/Ram/Memory2.h"
#include "LuxEngine/Memory/Memory_t.h"
#include "LuxEngine/Core/ConsoleOutput.h"
#include <cstring>


/*

			   CELL 0 ------------------.-.
			  .──────────────────────.  ¦ ¦                      CPU/GPU access       allocation types                                                CELL 0 ------------------.             50,331,648 bytes per buffer (Fixed count buffers only)
	   .-------> buffer   cellIndex  <--' ¦                                                                                                          .──────────────────────.  ¦             worst case:
	 .-¦ ------> type     address    <----'<-----.               R : read             SU : SHARED_UNIFORM                                      .----->  buffer   cellIndex  <--'                 RAM     | 905,969,664 (~906MB)   allocated    | 404,082 (~405KB) used
	 ¦ ¦      │  owners   cellSize   <-----------¦               W : write            SS : SHARED_STORAGE                                    .-¦ ---->  type     cellSize   <--.                 VRAM    | 603,979,776 (~604MB)   allocated    | 269,388 (~269KB) used
	 ¦ ¦      '────.   .─────────────'           ¦               - : none             DU : DEDICATED_UNIFORM                                 ¦ ¦     '──────────.   .───────'  ¦                           50,331,648 * (7-1) * n                (1 + 33 + 513 + 1025 + 2049 + 131071) * n
	 ¦ ¦   RAM     |  /                          ¦                                    DS : DEDICATED_STORAGE                                 ¦ ¦  VRAM / SHARED  \  |          ¦
	 ¦ '-- BUFFER 0| / TP0                       ¦                                                                                           ¦ '- BUFFER 0 TP7    \ |          ¦             Worst case + data structures count:
	 ¦    .────────|/ ─────────────.             ¦                                                                                           ¦   .─────────────────\|────.     ¦                 RAM     | 1,057,563,648 (~1.06GB) allocated   | 151,998,786 (~152MB) used
	 ¦    │  cell 0    cell class  <------.------¦ ------------------------------------------------------------------------------------------¦ -->  cell class   cell 0  │     ¦                 VRAM    | [no changes]                        | [no changes]
	 ¦    │  cell 1                │	  ¦      ¦                                                                                      .----¦ -->  allocBck type   cell 1  │     ¦                           a = (64 + sizeof(lux::ram::Cell_t)) = 384, b = (64 + sizeof(lux::vram::Cell_t)) = 288
	 ¦    │  cell 2                │	  ¦      ¦                                                                                      ¦    ¦   │               cell 2  │     ¦                           e = (6 + 384 + 24576 + 24576 + 24576 + 24576) = 98,694
	 ¦    │  ...                   │	  ¦      ¦                                                                                      ¦    ¦   │                ...    │     ¦                           905,969,664 + (a * e) + (b * e * 4) = 905,969,664 + 151,593,984 = 1,057,563,648 (~1.06GB)
	 ¦    '──────.      .──────────'	  ¦      ¦                                                                                      ¦    ¦   '──────.      .─────────'     ¦
	 ¦            \     |                 0      1    [ RAM ]                                               [ SHARED RAM ]              2    '-------.  │     /                2                                                                               [ VRAM ]
	 ¦             \    | _______________________________________________________ ___________________________________________________________________¦__│    /______________________________________      ___________________________________________________________________________________________________________________
	 ¦             │\   | ───────────────────────────────────────────────────────│───────────────────────────────────────────────────────────────────¦──│   /─────────────────────────────────────. │    │ .───────────────────────────────────────────────────────────────────────────────────────────────────────────────. │
	 ¦             │ \  |                                                        │                                                                   ^  │  /                                      │ │    │ │                                                                                                               │ │
	 '------------------ TP 0  | TP 1  | TP 2  | TP 3  | TP 4  | TP 5  | TP 6    │  TP 2  │ TP 6  │ TP 10 │ TP 14 │ TP 18 │ TP 22 │ TP 26 │ TP 3  │ TP 7│ / TP 11 │ TP 15 │ TP 19 │ TP 23 │ TP 27 │ │    │ │ TP 0  │ TP 4  │ TP 8  │ TP 12 │ TP 16 │ TP 20 │ TP 24 │ TP 1  │ TP 5  │ TP 9  │ TP 13 │ TP 17 │ TP 21 │ TP 25 │ │
				   │ │ \|──────|───────|───────|───────|───────|───────|──────   │  ──────|───────|───────|───────|───────|───────|───────|───────|─────│/|───────|───────|───────|───────|───────│ │    │ │ ──────|───────|───────|───────|───────|───────|───────|───────|───────|───────|───────|───────|───────|───────│ │
				   │ │   buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0   │  buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 │ │    │ │ buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 | buf 0 │ │
				   │ │   buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1   │  buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 │ │    │ │ buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 | buf 1 │ │
				   │ │   buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2   │  buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 │ │    │ │ buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 | buf 2 │ │
				   │ │   ...   | ...   | ...   | ...   | ...   | ...   | ...     │  ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   │ │    │ │ ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   | ...   │ │
				   │ │                                                           │                                                                                                                │ │    │ │                                                                                                               │ │
				   │ '.__________________________________________________________│_______________________________________________________________________________________________________________.' │    │ '._____________________________________________________________________________________________________________.' │
				   '─────────────────────────────────────────────────────────────'──────────────────────────────────────────────────────────────────────────────────────────────────────────────────'    '───────────────────────────────────────────────────────────────────────────────────────────────────────────────────'
										  ¦      1                                                                                      2                                      2
										  ¦      ¦                                                                                      ¦                                      ¦
										  ¦      ¦                                                                                      ¦--------------------------------------'
─────────────────────────────────────────────────¦ ─────────────────────────────────────────────────────────────────────────────────────¦ ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
					 │                           ^                                                                                      ^                                                                                                                                                                                  │
   allocate          │                    lux::ram::alloc--                                                                        lux::rem::alloc                                                                                                          lux::rem::alloc                                                │  allocate
   CPU access        │   R/W   ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W        R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W               -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   │  CPU access
   GPU access        │    -    ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -         R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦   R   ¦   R   ¦   R   ¦   R   ¦   R   ¦   R   ¦   R               R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦  R/W  ¦   R   ¦   R   ¦   R   ¦   R   ¦   R   ¦   R   ¦   R   │  GPU access
   allocation        │    -    ¦   -   ¦   -   ¦   -   ¦   -   ¦   -   ¦   -         SS   ¦  SS   ¦  SS   ¦  SS   ¦  SS   ¦  SS   ¦  SS   ¦  SU   ¦  SU   ¦  SU   ¦  SU   ¦  SU   ¦  SU   ¦  SU               DS   ¦  DS   ¦  DS   ¦  DS   ¦  DS   ¦  DS   ¦  DS   ¦  DU   ¦  DU   ¦  DU   ¦  DU   ¦  DU   ¦  DU   ¦  DU   │  allocation
					 │         ¦       ¦       ¦       ¦       ¦       ¦                  ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦                        ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       ¦       │
   cells per buffer	 │  1572864¦98,304 ¦49,152 ¦24,576 ¦  384  ¦   6   ¦   1       1572864¦98,304 ¦49,152 ¦24,576 ¦  384  ¦   6   ¦   1   ¦1572864¦98,304 ¦49,152 ¦24,576 ¦  384  ¦   6   ¦   1             1572864¦98,304 ¦49,152 ¦24,576 ¦  384  ¦   6   ¦   1   ¦1572864¦98,304 ¦49,152 ¦24,576 ¦  384  ¦   6   ¦   1   │  cells per buffer
   bytes  per  cell  │    32   ¦  512  ¦ 1,024 ¦ 2,048 ¦131,072¦8388608¦   ?         32   ¦  512  ¦ 1,024 ¦ 2,048 ¦131,072¦8388608¦   ?   ¦  32   ¦  512  ¦ 1,024 ¦ 2,048 ¦131,072¦8388608¦   ?               32   ¦  512  ¦ 1,024 ¦ 2,048 ¦131,072¦8388608¦   ?   ¦  32   ¦  512  ¦ 1,024 ¦ 2,048 ¦131,072¦8388608¦   ?   │  bytes  per  cell
		   ↓         │                                                                                                                                                                                                                                                                                                     │
───────────¦ ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
		   ¦                              0
		   '------------------------------'


*/







//TODO draw graph of the allocation/reallocation process
//TODO MAKE THE ARRAY COMPILE TIME

// Main ------------------------------------------------------------------------------------------------------------------------ //








#pragma warning( disable : 4227 )    //"Anachronism used: qualifiers on reference are ignored"
namespace lux::ram{
	extern MemBufferType* buffers;	//Allocated buffers
	extern uint32 allocated;			//TODO remove


	Cell_t* alloc_internal(const uint64 vSize, const CellClass vClass = CellClass::AUTO);
	inline Cell_t* alloc_call(const uint64 vSize, CellClass vClass = CellClass::AUTO){
		//The count cannot be zero. If it is, allocate 1 byte and set the count variable back to zero
		if(!vSize) {
			Cell_t* cell = alloc_internal(1, vClass);
			cell->cellSize = 0;
			return cell;
		}
		else return alloc_internal(vSize, vClass);
	}

	void evaluateCellClass(const uint64 vSize, CellClass& pClass);


	//TODO UNROLL
	template<class type> inline void init_memory(void* const vAddr, const uint64 vSize, const type& pValue){
		for(uint32 i = 0; i < vSize; i+=sizeof(type)) memcpy((char*)vAddr + i, &pValue, sizeof(type));
	}







	// Block allocation -------------------------------------------------------------------------------------------------------- //








	//Allocates a block of memory without initializing it
	//e.g.   lux::ram::ptr<int32> p = lux::ram::AllocBck(302732);
	//Specify the type as template argument to initialize it
	inline Cell_t* allocBck(const uint64 vSize, CellClass vClass = CellClass::AUTO) {
		evaluateCellClass(vSize, vClass);
		return alloc_call(vSize, vClass);
	}

	//Allocates a block of memory and initializes each element with the pValue value
	//e.g.   lux::ram::ptr<float> p = lux::ram::AllocBck<float>(302732, 15.0f);
	//You need to specify the type when calling this function
	//If you don't, the elements will remain uninitialized
	template<class type> inline ptr<type> allocBck(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO){
		evaluateCellClass(vSize, vClass);
		param_error_2(vSize % sizeof(type) != 0, vSize, "The type is %llu bytes large and vSize (%llu) is not a multiple of it. If not zero, vSize must be a multiple of the type's size", sizeof(type), vSize);
		Cell_t* cell = ram::alloc_call(vSize, vClass);
		init_memory<type>(cell->address, vSize, pValue);
		return cell;

	}

	//Allocates a block of memory and initializes it by calling the default constructor of each element
	//e.g.   lux::ram::ptr<int32> p = lux::ram::AllocBck<int32>(302732);
	//You need to specify the type when calling this function
	//If you don't, the elements will remain uninitialized
	template<class type> inline ptr<type> AllocBck(const uint64 vSize, const CellClass vClass = CellClass::AUTO){
		return allocBck<type>(vSize, type( ), vClass);
	}








	// Array allocation -------------------------------------------------------------------------------------------------------- //








	//Allocates a block of memory containing <vNum> <vSize>-bytes elements without initializing them
	//e.g.   lux::ram::ptr<float> p = lux::ram::allocArr(sizeof(float), 8);
	//Specify the type as template argument to initialize the elements
	inline Cell_t* allocArr(const uint64 vSize, const uint64 vNum, CellClass vClass = CellClass::AUTO) {
		evaluateCellClass(vSize * vNum, vClass);
		return alloc_call(vSize * vNum, vClass);
	}

	//Allocates a block of memory containing <vNum> <vSize>-bytes elements and initializes each element with the pValue value
	//e.g.   lux::ram::ptr<float> p = lux::ram::allocArr<float>(sizeof(float), 8, 3.14159f);
	//You need to specify the type when calling this function
	//If you don't, the elements will remain uninitialized
	template<class type> inline ptr<type> allocArr(uint64 vSize, const uint64 vNum, const type& pValue, CellClass vClass = CellClass::AUTO){
		uint64 size = vSize * vNum;
		evaluateCellClass(size, vClass);
		if(vClass == CellClass::CLASS_0) size = multipleOf(size, sizeof(type));
		Cell_t* cell = ram::alloc_call(size, vClass);
		init_memory<type>(cell->address, size, pValue);
		return cell;

	}

	//Allocates a block of memory containing <vNum> <vSize>-bytes elements and calls the default constructor of each element
	//e.g.   lux::ram::ptr<int32> p = lux::ram::allocArr<int32>(sizeof(int32), 27);
	//You need to specify the type when calling this function
	//If you don't, the elements will remain uninitialized
	template<class type> inline ptr<type> allocArr(const uint64 vSize, const uint64 vNum, CellClass vClass = CellClass::AUTO) {
		return allocArr<type>(vSize, vNum, type( ), vClass);
	}








	// Block reallocation ------------------------------------------------------------------------------------------------------ //
	void free(Cell_t* pCell);
	template<class t> static inline void free(ptr<t>& pPtr) { ram::free(pPtr.cell); }







	//Reallocates a block of memory without initializing it
	//e.g.   lux::ram::reallocBck(p, 100);
	//Specify the type as template argument to initialize it
	void reallocBck(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO);
	template<class t> static inline void reallocBck(const ptr<t>& pPtr, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO) {
		ram::reallocBck(pPtr.cell, vSize, vCellClass);
	}


	//Reallocates a block of memory and initializes each of the new elements with the pValue value
	//e.g.   lux::ram::reallocBck<float>(p, 100, 0.1f);
	//You need to specify the type when calling this function
	//If you don't, the elements will remain uninitialized
	template<class type> static inline void reallocBck(ptr<type>& pPtr, const uint64 vSize, const type& pValue, CellClass vCellClass = CellClass::AUTO){
		evaluateCellClass(vSize, vCellClass);

		if(!pPtr.address) { [[unlikely]]						//If the pointer has not been allocated
			pPtr = alloc_call(vSize, vCellClass);					//Allocate it
			init_memory(pPtr.end( ), vSize, pValue);				//And initialize the memory
			return;
		}
		else { 										//If it's allocated
			int64 d = vSize - pPtr.size( );							//Calculate the differenze of size between the current size and the new size
			if(d < 0) [[unlikely]] pPtr.cell->cellSize = vSize;		//If the new size is smaller, change the cellSize variable and return
			else if(d > 0) { [[likely]]								//If it's larger
				if(vSize <= (int64)vCellClass) { [[likely]]				//But not larger than the maximum cell size
					pPtr.cell->cellSize = vSize;							//Change the cellSize variable
					init_memory(pPtr.end( ), (uint64)d, pValue);			//And initialize the new elements
				}
				else {										//If it's also larger than the cell
					ram::ptr<type> ptr_ = alloc_call(vSize, vCellClass);	//Allocate a new pointer
					memcpy(ptr_, pPtr, pPtr.size( ));						//Copy the old data
					init_memory(pPtr.end(), (uint64)d, pValue);				//Initialize the new elements
					ram::free(pPtr);										//Free the old cell
					*pPtr.cell = *ptr_.cell;								//Overwrite the cell itself. This is necessary in order to keep the pointers updated
					pPtr = ptr_;											//Overwrite the old pointer
				}
			}
			else [[unlikely]] return;								//If it has the same size, do nothing
		}
	}


	////Reallocates a block of memory and initializes the new elements by calling their default constructor
	////e.g.   lux::ram::reallocBck<float>(p, 100);
	////You need to specify the type when calling this function
	////If you don't, the new elements will remain uninitialized
	//template<class type> static inline void reallocBck(ptr<type>& pPtr, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO){
	//	reallocBck<type>(pPtr, vSize, type( ), vCellClass);
	//}







	//Reallocates a block of memory without initializing it
	//e.g.   lux::ram::reallocBck(p, 100);
	//Specify the type as template argument to initialize it
	void reallocArr(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO);
	template<class t> static inline void reallocArr(const ptr<t>& pPtr,const uint64 vSize, const uint64 vCount, const CellClass vCellClass = CellClass::AUTO) {
		ram::reallocBck(pPtr, vSize * vCount, vCellClass);
	}


	//Reallocates a block of memory and initializes each of the new elements with the pValue value
	//e.g.   lux::ram::reallocBck<float>(p, 100, 0.1f);
	//You need to specify the type when calling this function
	//If you don't, the elements will remain uninitialized
	template<class type> static inline void reallocArr(ptr<type>& pPtr, const uint64 vSize, const uint64 vCount, const type& pValue, CellClass vCellClass = CellClass::AUTO){
		ram::reallocBck<type>(pPtr, vSize * vCount, pValue, vCellClass);
	}





	void init( );
	void breakMemoryPool( );
}
#pragma warning( default : 4227 )
#endif