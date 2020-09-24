#ifndef LUX_H_MEMORY
#define LUX_H_MEMORY


#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory2.h"
#include "LuxEngine/Memory/Memory_t.h"
#include "LuxEngine/Core/ConsoleOutput.h"



/*

			   CELL 0 ------------------.-.
			  .──────────────────────.  ¦ ¦                      CPU/GPU access       allocation types                                                CELL 0 ------------------.             50,331,648 bytes per buffer (Fixed size buffers only)
	   .-------> buffer   cellIndex  <--' ¦                                                                                                          .──────────────────────.  ¦             worst case:
	 .-¦ ------> type     address    <----'<-----.               R : read             SU : SHARED_UNIFORM                                      .----->  buffer   cellIndex  <--'                 RAM     | 905,969,664 (~906MB)   allocated    | 404,082 (~405KB) used
	 ¦ ¦      │  owners   cellSize   <-----------¦               W : write            SS : SHARED_STORAGE                                    .-¦ ---->  type     cellSize   <--.                 VRAM    | 603,979,776 (~604MB)   allocated    | 269,388 (~269KB) used
	 ¦ ¦      '────.   .─────────────'           ¦               - : none             DU : DEDICATED_UNIFORM                                 ¦ ¦     '──────────.   .───────'  ¦                           50,331,648 * (7-1) * n                (1 + 33 + 513 + 1025 + 2049 + 131071) * n
	 ¦ ¦   RAM     |  /                          ¦                                    DS : DEDICATED_STORAGE                                 ¦ ¦  VRAM / SHARED  \  |          ¦
	 ¦ '-- BUFFER 0| / TP0                       ¦                                                                                           ¦ '- BUFFER 0 TP7    \ |          ¦             Worst case + data structures size:
	 ¦    .────────|/ ─────────────.             ¦                                                                                           ¦   .─────────────────\|────.     ¦                 RAM     | 1,057,563,648 (~1.06GB) allocated   | 151,998,786 (~152MB) used
	 ¦    │  cell 0    cell class  <------.------¦ ------------------------------------------------------------------------------------------¦ -->  cell class   cell 0  │     ¦                 VRAM    | [no changes]                        | [no changes]
	 ¦    │  cell 1                │	  ¦      ¦                                                                                      .----¦ -->  allocUB type   cell 1  │     ¦                           a = (64 + sizeof(lux::ram::Cell_t)) = 384, b = (64 + sizeof(lux::vram::Cell_t)) = 288
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
   allocate          │                     lux::ram::allocUB                                                                        lux::rem::allocUB                                                                                                           lux::rem::allocUB                                                │  allocate
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









// Main ------------------------------------------------------------------------------------------------------------------------ //








#pragma warning( disable : 4227 )    //"Anachronism used: qualifiers on reference are ignored"
namespace lux::ram{
	extern MemBufferType* buffers;	//Allocated buffers
	extern uint32 allocated;			//TODO remove


	Cell_t* alloc_internal(const uint64 vSize, const CellClass vClass = CellClass::AUTO);
	inline Cell_t* alloc_call(const uint64 vSize, CellClass vClass = CellClass::AUTO, const bool vForceDedicatedBuffer = false){
		//The size cannot be zero. If it is, allocate 1 byte and set the size variable back to zero
		if(!vSize) {
			Cell_t* cell = alloc_internal(1, vClass);
			cell->cellSize = 0;
			return cell;
		}
		else return alloc_internal(vSize, vClass);
	}

	void evaluateCellClass(const uint64 vSize, CellClass& pClass);








	// Block allocation -------------------------------------------------------------------------------------------------------- //








	//"Allocate Uninitialized Block"
	//Allocates a block of memory without initializing it
	inline Cell_t* allocUB(const uint64 vSize, CellClass vClass = CellClass::AUTO) {
		evaluateCellClass(vSize, vClass);
		return alloc_call(vSize, vClass, false);
	}

	//"Allocate Default Block"
	//Allocates a block of memory and initializes it by calling the default constructor of each element
	//You need to specify the type when calling this function
	//e.g.   lux::ram::ptr<int32> p = lux::ram::AllocDB<int32>(302732);
	//The type can be omitted only if the size is 0
	template<class type> inline ptr<type> allocVB(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO){
		evaluateCellClass(vSize, vClass);
		param_error_2(vSize % sizeof(type) != 0, vSize, "The type is %llu bytes large and vSize (%llu) is not a multiple of it. If not zero, vSize must be a multiple of the type's size", sizeof(type), vSize);
		Cell_t* cell = ram::alloc_call(vSize, vClass);
		for(uint32 i = 0; i < ((uint32)vClass ? (uint32)vClass : vSize); i+=sizeof(type)) memcpy((char*)cell->address + i, &pValue, sizeof(type));
		return cell;

	}

	//"Allocate Default Block"
	//Allocates a block of memory and initializes it by calling the default constructor of each element
	//You need to specify the type when calling this function
	//e.g.   lux::ram::ptr<int32> p = lux::ram::AllocDB<int32>(302732);
	//The type can be omitted only if the size is 0
	template<class type> inline ptr<type> AllocDB(const uint64 vSize, const CellClass vClass = CellClass::AUTO){
		return allocVB<type>(vSize, type( ), vClass);
	}








	// Array allocation -------------------------------------------------------------------------------------------------------- //








	//"Allocate Uninitialized Array"
	//Allocates a block of memory containing <vNum> <vSize>-bytes elements without initializing them
	inline Cell_t* allocUA(const uint64 vSize, const uint64 vNum, CellClass vClass = CellClass::AUTO) {
		evaluateCellClass(vSize * vNum, vClass);
		return alloc_call(vSize * vNum, vClass, false);
	}

	//"Allocate Value Array"
	//Allocates a block of memory containing <vNum> <vSize>-bytes elements and initializes each element with the pValue value
	//You MUST specify the type when calling this function
	//e.g.   lux::ram::ptr<float> p = lux::ram::allocDA<int32>(sizeof(float), 8, 3.14159f);
	//Not specifying the type can lead to runtime errors or other stuff you don't wanna see
	template<class type> inline ptr<type> allocVA(uint64 vSize, const uint64 vNum, const type& pValue, CellClass vClass = CellClass::AUTO){
		uint64 size = vSize * vNum;
		evaluateCellClass(size, vClass);
		if(vClass == CellClass::CLASS_0) size = multipleOf(size, sizeof(type));
		Cell_t* cell = ram::alloc_call(size, vClass);
		for(uint32 i = 0; i < ((uint32)vClass ? (uint32)vClass : size); i+=sizeof(type)) memcpy((char*)cell->address + i, &pValue, sizeof(type));
		return cell;

	}

	//"Allocate Default Array"
	//Allocates a block of memory containing <vNum> <vSize>-bytes elements and calls the default constructor of each element
	//You need to specify the type when calling this function
	//e.g.   lux::ram::ptr<int32> p = lux::ram::allocDA<int32>(sizeof(int32), 27);
	template<class type> inline ptr<type> allocDA(const uint64 vSize, const uint64 vNum, CellClass vClass = CellClass::AUTO) {
		return allocVA<type>(vSize, vNum, type( ), vClass);
	}








	// Reallocate and free ----------------------------------------------------------------------------------------------------- //








	void free(Cell_t* pCell);
	template<class t> static inline void free(ptr<t>& pPtr) { ram::free(pPtr.cell); }


	void reallocUB(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO);
	template<class t> static inline void reallocUB(ptr<t>& const pPtr, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO) {
		ram::reallocUB(pPtr.cell, vSize, vCellClass);
	}



	//TODO initialize only new elements
	template<class t> static inline void reallocDB(ptr<t>& pPtr, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO){
		if(!pPtr.address) [[unlikely]] {
			pPtr = AllocDB<t>(vSize, vCellClass);
			return;
		}
		if((vCellClass == CellClass::AUTO && vSize < (uint32)pPtr.cell->bufferType->cellClass) || (vCellClass == pPtr.cell->bufferType->cellClass && vSize < (uint32)vCellClass)) [[likely]] pPtr.cell->cellSize = vSize;
		else if(vSize != pPtr.size( )) [[unlikely]] {
			ram::ptr<t> ptr_ = AllocDB<t>(vSize, vCellClass);
			memcpy(ptr_, pPtr, pPtr.size( ));
			ram::free(pPtr);
			pPtr = ptr_;
		}
	}




	void init( );
	void breakMemoryPool( );
}
#pragma warning( default : 4227 )
#endif