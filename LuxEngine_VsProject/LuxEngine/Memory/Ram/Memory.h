#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Memory/Ram/Memory2.h"
#include "LuxEngine/Memory/Memory_t.h"





/*
    A cell is a fixed-size partition of memory inside an allocated buffer
    Cells are allocated in RAM memory with lux::ram::alloc, in VRAM or shared memory with lux::rem::alloc

────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

                                RAM                        SHARED                                  VRAM
                                                     uniform   storage                      uniform   storage
                 .─────────────────────────────────.────────────────────.            .──────────────────────────────────.
        Buffer 0 │ cell0  cell1  cell2  cell3..    ¦ cell0.. | cell0..  │            │ cell0  cell1.. | cell0  cell1..  │
                 │─────────────────────────────────¦─────────|──────────│            │────────────────|─────────────────│
        Buffer 1 │ cell0  cell1  cell2  cell3..    ¦ cell0.. | cell0..  │            │ cell0  cell1.. | cell0  cell1..  │
        ...      │─────────────────────────────────¦─────────|──────────│            │────────────────|─────────────────│
                 │                                 ¦                    │            │                                  │
                 '───────────────↑─────────────────'──↑────↑────↑────↑──'            '────────↑───────────────↑─────────'
                                 ¦                    ¦    R    ¦   R/W                       R              R/W
                                R/W                  R/W   ¦   R/W   ¦                        ¦               ¦
                                 ¦                    ¦    '----¦ ---'------------------------'---------------¦
                                 ¦------ map() -------'---------'                                             ¦
                                CPU                                                                          GPU

────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

    lux::            │     ram::alloc()       rem::alloc()   | rem::alloc()         rem::alloc()      | rem::alloc()
    lux::AllocType:: │         -              SHARED_UNIFORM | SHARED_STORAGE       DEDICATED_UNIFORM | DEDICATED_STORAGE

*/







#pragma warning( disable : 4227 )    //"Anachronism used: qualifiers on reference are ignored"
namespace lux::ram{
	Cell alloc(const uint64 vSize, const CellClass vCellClass);
	void realloc(Cell& const pCell, const uint64 vSize, const CellClass vCellClass = CellClass::AUTO);
	void free(Cell pCell);


	extern Array<MemBufferType> buffers;	//Allocated buffers
	void init( );
	void breakMemoryPool( );
}
#pragma warning( default : 4227 )
