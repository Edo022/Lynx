#ifndef LUX_H_MEMORY
#define LUX_H_MEMORY


#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory2.h"
#include "LuxEngine/Memory/Memory_t.h"

//TODO just duplicate the lux array or use a normal array

//TODO throw exception when an error occurs

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
	 ¦    │  cell 1                │	  ¦      ¦                                                                                      .----¦ -->  alloc type   cell 1  │     ¦                           a = (64 + sizeof(lux::ram::Cell_t)) = 384, b = (64 + sizeof(lux::vram::Cell_t)) = 288
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
   allocate          │                     lux::ram::alloc                                                                        lux::rem::alloc                                                                                                           lux::rem::alloc                                                │  allocate
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










#pragma warning( disable : 4227 )    //"Anachronism used: qualifiers on reference are ignored"
namespace lux::ram{

	//TODO use those functions everythere instead of randomly calling constructors on uninitialized data
	//Allocates a block of memory without initializing it
	Cell_t* alloc(const uint64 vSize, CellClass vCellClass = CellClass::AUTO, const bool vForceDedicatedBuffer = false);
	//Allocates a memory block large enough to contain <vNum> <vSize>-bytes elements and initializes each element with the value of pValue
	template<class type> ptr<type> vAlloc(const uint64 vSize, const uint64 vNum, const type& pValue, const CellClass vClass = CellClass::AUTO){
		//ptr<type> ptr = ram::alloc(vSize * vNum, vClass);
		ptr<type> ptr = ram::alloc(vSize * vNum, (vSize * vNum) < (uint64)vClass ? vClass : CellClass::AUTO);
		//for(auto e : ptr) e = pValue;
		//for(uint32 i = 0; i < vSize; i++) ptr[i] = type( );
		for(uint32 i = 0; i < vSize; i++){
			memcpy(&ptr[i], &pValue, sizeof(type));
		}
		return ptr;
	}

	//TODO rename and reorder this mess
	////TODO rename avAlloc and adAlloc
	//template<class type> ptr<type> vAlloc2(const uint64 vSize, const type& pValue, const CellClass vClass = CellClass::AUTO){ ptr<type> ptr = ram::alloc(vSize * vNum, vClass); for(auto e : ptr) e = pValue; return ptr; }
	//Allocates a memory block large enough to contain <vNum> <vSize>-bytes elements and calls the default constructor of each element
	template<class type> ptr<type> dAlloc(const uint64 vSize, const uint64 vNum, const CellClass vClass = CellClass::AUTO){
		//ram::ptr<type> ptr = ram::alloc(vSize * vNum, vClass);
		ram::ptr<type> ptr = ram::alloc(vSize * vNum, (vSize * vNum) < (uint64)vClass ? vClass : CellClass::AUTO);
		//for(auto e : ptr) e = type( );
		//for(type* a = ptr; a++; a != ptr.end( )) {
		type* _type = new type();
		for(uint32 i = 0; i < vSize; i++) {
			//*a = type( );
			//ptr[i] = type( );

			memcpy(&ptr[i], _type, sizeof(type));
		}
		//free(_type);
		delete(_type);
		//TODO for some reason, returning the ptr direclty makes it to cast to a normal pointer
		//return ptr;
		return (ram::ptr<type>)ptr;
	}
	template<class type> ptr<type> dAlloc2(const uint64 vSize, const CellClass vClass = CellClass::AUTO){
		ptr<type> ptr = ram::alloc(vSize, vClass);
		//for(auto e : ptr) e = type( );
		//for(uint32 i = 0; i < vSize; i++) ptr[i] = type( );
		type* _type = new type();
		for(uint32 i = 0; i < vSize; i++) {
			memcpy(&ptr[i], _type, sizeof(type));
		}
		//free(_type);
		delete(_type);

		return ptr;
	}
	template<class type> Cell_t* dAlloc2c(const uint64 vSize, const CellClass vClass = CellClass::AUTO){
		Cell_t* ptr = ram::alloc(vSize, vClass);
		//for(auto e : ptr) e = type( );
		//for(type* a = (type*)ptr->address; a++; a != (type*)((uint64)ptr->address + ptr->cellSize)) *a = type( );
		type* _type = new type();
		for(uint32 i = 0; i < vSize; i++) {
			//((type*)ptr->address)[i] = type( );
			memcpy(&((type*)ptr->address)[i], _type, vSize);
		}
		//free(_type);
		delete(_type);
		return ptr;
	}
	//non può.


	void free(Cell_t* pCell);
	void realloc(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO);
	template<class t> static inline void free(ptr<t>& pPtr) { ram::free(pPtr.cell); }


	template<class t> static inline void realloc(ptr<t>& const pPtr, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO) {
		ram::realloc(pPtr.cell, vSize, vCellClass);
	}
	template<class t> static inline void dRealloc(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO){
		if(!pCell->address) [[unlikely]] {
			//pCell = dAlloc2<t>(vSize, vCellClass);
			pCell = dAlloc2c<t>(vSize, vCellClass);
			return;
		}
		if((vCellClass == CellClass::AUTO && vSize < (uint32)pCell->bufferType->cellClass) || (vCellClass == pCell->bufferType->cellClass && vSize < (uint32)vCellClass)) [[likely]] pCell->cellSize = vSize;
		else if(vSize != pCell->cellSize) [[unlikely]] {
			Cell_t* cell = dAlloc2c<t>(vSize, vCellClass);
			memcpy(cell, pCell, pCell->cellSize);

			ram::free(pCell);
			pCell->address = cell->address; pCell->buffer = cell->buffer; pCell->cellSize = cell->cellSize; pCell->bufferType = cell->bufferType; pCell->cellIndex = cell->cellIndex;
		}
	}
	template<class t> static inline void dRealloc(ptr<t>& pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO){
		dRealloc<t>(pCell.cell, vSize, vCellClass);
		//dRealloc(pCell.cell, vSize);
	}






	extern MemBufferType* buffers;	//Allocated buffers
	extern uint32 allocated;			//TODO remove
	void init( );
	void breakMemoryPool( );
}
#pragma warning( default : 4227 )
#endif