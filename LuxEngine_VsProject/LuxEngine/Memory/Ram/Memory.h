#ifndef LUX_H_MEMORY
#define LUX_H_MEMORY


#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory2.h"
#include "LuxEngine/Memory/Memory_t.h"
#include "LuxEngine/Core/ConsoleOutput.h"


//TODO throw exception when an error occurs
//TODO decrease default map chunkSize


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

	Cell_t* alloc__(const uint64 vSize, const CellClass vClass = CellClass::AUTO);
	//Allocates a block of memory without initializing it
	inline Cell_t* alloc(const uint64 vSize, CellClass vClass = CellClass::AUTO, const bool vForceDedicatedBuffer = false){
		luxDebug(if(vClass != CellClass::AUTO && (uint32)vClass < vSize) param_error(vClass, "The cell class must be large enought to contain the cell. Use lux::CellClass::AUTO to automatically choose it"));
		luxDebug(if(vSize > 0xFFFFffff) param_error(vSize, "The cell size cannot exceed 0xFFFFFFFF bytes"));


		//Set cell class if CellClass::AUTO was used or set it to custom size buffer if vForceDedicatedBuffer is true
		if(vForceDedicatedBuffer) vClass = CellClass::CLASS_0;
		else if(vClass == CellClass::AUTO) {
			/**/ if(vSize <= (uint32)CellClass::CLASS_A) [[likely]] vClass = CellClass::CLASS_A;
			else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]] vClass = CellClass::CLASS_B;
			else if(vSize <= (uint32)CellClass::CLASS_C) [[unlikely]] vClass = CellClass::CLASS_C;
			else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]] vClass = CellClass::CLASS_D;
			else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] vClass = CellClass::CLASS_Q;
			else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] vClass = CellClass::CLASS_L;
			else [[likely]] vClass = CellClass::CLASS_0;
		}


		if(!vSize) {
			Cell_t* cell = alloc__(vSize, vClass);
			cell->cellSize = 0;
			return cell;
		}
		else return alloc__(vSize, vClass);
	}



	template<class type> Cell_t* AllocVAc(const uint64 vSize, const uint64 vNum, const type& pValue, const CellClass vClass = CellClass::AUTO){
		Cell_t* cell = ram::alloc(vSize * vNum, vClass);
		//Cell_t* cell = ram::alloc(vSize * vNum, (vSize * vNum) < (uint64)vClass ? vClass : CellClass::AUTO);
		for(uint32 i = 0; i < vSize; i++) memcpy(&((type*)cell->address)[i], &pValue, sizeof(type));
		return cell;
	}
	//"Allocate Value Array"
	//Allocates a block of memory containing <vNum> <vSize>-bytes elements and initializes each element with the pValue value
	//You MUST specify the type when calling this function
	//e.g.   lux::ram::ptr<float> p = lux::ram::AllocDA<int32>(sizeof(float), 8, 3.14159f);
	//Not specifying the type can lead to runtime errors or other stuff you don't wanna see
	template<class type> ptr<type> AllocVA(const uint64 vSize, const uint64 vNum, const type& pValue, const CellClass vClass = CellClass::AUTO){ return AllocVAc<type>(vSize, vNum, pValue, vClass); }




	//TODO create object in allocated memory and skip first copy
	template<class type> Cell_t* AllocDAc(const uint64 vSize, const uint64 vNum, const CellClass vClass = CellClass::AUTO){
		Cell_t* cell = ram::alloc(vSize * vNum, vClass);//Allocate a new cell (If the cellSize is too small, throw an error)
		//Cell_t* cell = ram::alloc(vSize * vNum, (vSize * vNum) < (uint64)vClass ? vClass : CellClass::AUTO);//Allocate a new cell (If the cellSize is too small, throw an error)
		type* _type = new type( );																		   //Create an object with the default value (it's created in the allocated address to save space and performance)
		for(uint32 i = 0; i < vSize; i++) memcpy(&((type*)cell->address)[i], _type, sizeof(type));		   //For each element, memcpy the value in its address
		delete(_type);																					   //Return the inizialized cell
		return cell;
	}
	//"Allocate Default Array"
	//Allocates a block of memory containing <vNum> <vSize>-bytes elements and calls the default constructor of each element
	//You need to specify the type when calling this function
	//e.g.   lux::ram::ptr<int32> p = lux::ram::AllocDA<int32>(sizeof(int32), 27);
	template<class type> ptr<type> inline AllocDA(const uint64 vSize, const uint64 vNum, const CellClass vClass = CellClass::AUTO) { return ram::AllocDAc<type>(vSize, vNum, vClass); }




	//TODO create object in allocated memory and skip first copy
	template<class type> Cell_t* AllocDBc(const uint64 vSize, const CellClass vClass = CellClass::AUTO) {
		Cell_t* cell = ram::alloc(multipleOf(vSize, sizeof(type)), vClass);							//Allocate a new cell
		type* _type = new type( );																	//Create an object with the default value (it's created in the allocated address to save space and performance)
		for(uint32 i = 0; i < vSize; i++) memcpy(&((type*)cell->address)[i], _type, sizeof(type));	//For each element, memcpy the value in its address
		delete(_type); return cell;																	//Return the inizialized cell
	}
	//"Allocate Default Block"
	//Allocates a block of memory and initializes it by calling the default constructor of each element
	//You need to specify the type when calling this function
	//e.g.   lux::ram::ptr<int32> p = lux::ram::AllocDB<int32>(302732);
	//The type can be omitted only if the size is 0
	template<class type> ptr<type> inline AllocDB(const uint64 vSize, const CellClass vClass = CellClass::AUTO){ return ram::AllocDBc<type>(vSize, vClass); }




	void free(Cell_t* pCell);
	void realloc(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO);
	template<class t> static inline void free(ptr<t>& pPtr) { ram::free(pPtr.cell); }


	template<class t> static inline void realloc(ptr<t>& const pPtr, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO) {
		ram::realloc(pPtr.cell, vSize, vCellClass);
	}
	template<class t> static inline void dRealloc(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO){
		if(!pCell->address) [[unlikely]] {
			pCell = AllocDBc<t>(vSize, vCellClass);
			return;
		}
		if((vCellClass == CellClass::AUTO && vSize < (uint32)pCell->bufferType->cellClass) || (vCellClass == pCell->bufferType->cellClass && vSize < (uint32)vCellClass)) [[likely]] pCell->cellSize = vSize;
		else if(vSize != pCell->cellSize) [[unlikely]] {
			Cell_t* cell = AllocDBc<t>(vSize, vCellClass);
			memcpy(cell, pCell, pCell->cellSize);

			ram::free(pCell);
			//pCell->address = cell->address; pCell->buffer = cell->buffer; pCell->cellSize = cell->cellSize; pCell->bufferType = cell->bufferType; pCell->cellIndex = cell->cellIndex;
			*pCell = *cell;
		}
	}
	template<class t> static inline void dRealloc(ptr<t>& pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO){
		dRealloc<t>(pCell.cell, vSize, vCellClass);
	}






	extern MemBufferType* buffers;	//Allocated buffers
	extern uint32 allocated;			//TODO remove
	void init( );
	void breakMemoryPool( );
}
#pragma warning( default : 4227 )
#endif