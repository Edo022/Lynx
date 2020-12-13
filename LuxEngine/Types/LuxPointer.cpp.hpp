#ifdef __INTELLISENSE__
#include "LuxPointer.hpp"
#endif
//TODO fix all the documentation








template<class type> void lux::ram::ptr<type, alloc>::evaluateCellClass(const uint64 vSize, CellClass& pClass){
    if(pClass != CellClass::AUTO && (uint32)pClass % LuxMemOffset == 1) {	//Check AT_LEAST values (normal class values + 1)
        if(vSize > ((uint32)pClass - 1)) pClass = CellClass::AUTO;				//If the class is too small, set it to AUTO
        else pClass = (CellClass)((uint64)pClass - 1);							//If it's large enough, assign the normal class value
    }
    if(pClass == CellClass::AUTO) { [[likely]]								//Choose cell class if it's AUTO
        if(vSize <= (uint32)CellClass::CLASS_A) [[likely]]	  pClass = CellClass::CLASS_A;
        else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]]	  pClass = CellClass::CLASS_B;
        else if(vSize <= (uint32)CellClass::CLASS_C) [[likely]]	  pClass = CellClass::CLASS_C;
        else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]] pClass = CellClass::CLASS_D;
        else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] pClass = CellClass::CLASS_Q;
        else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] pClass = CellClass::CLASS_L;
        else													  pClass = CellClass::CLASS_0;
    }

    luxCheckParam(vSize > 0xFFFFffff, vSize, "Cell size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", vSize);
    luxCheckParam((uint32)pClass < vSize, pClass,
        "Requested %lu-bytes class for %llu-bytes allocation. The cell class must be large enought to contain the cell. %s",
        (uint32)pClass, vSize, "Use lux::CellClass::AUTO to automatically choose it"
    );
}








//TODO CHECK MEMORY FULL
template<class type> void lux::ram::ptr<type, alloc>::alloc_(const uint64 vSize, const CellClass vClass){
    uint32 typeIndex = lux::__pvt::classIndexFromEnum(vClass);							//Get buffer index from type and class
    Map_NMP_S<MemBuffer, uint32>& subBuffers = (lux::ram::buffers[typeIndex].buffers);	//Get list of buffers where to search for a free cell
    uint32 cellIndex;
    if((uint32)vClass){																	//If the cell is a fixed count cell
        uint64 cellNum = lux::__pvt::bufferSize / (uint32)vClass;							//Get the maximum number of cells in each buffer
        for(uint32 i = 0; i < subBuffers.size( ); i++){										//Search for a suitable buffer
            if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {			//If a buffer is valid and it has a free cell
                cellIndex = subBuffers[i].cells.add(Cell_t{											//Create a new cell in the buffer
                    .cellSize = vSize,
                    .bufferType = &lux::ram::buffers[typeIndex]
                });
                cell = &subBuffers[i].cells[cellIndex];
                cell->buffer = &subBuffers[i];														//and set its buffer, index and address
                cell->cellIndex = cellIndex;
                this->address = (type*)(cell->address = (void*)((uint8*)(cell->buffer->memory) + getCellOffset(cell)));
                return;
            }
        }
    }{																							//If there are no free buffers or the cell is a custom count cell
        uint32 bufferIndex, cellsNum = (uint32)vClass ? lux::__pvt::bufferSize / (uint32)vClass : 1;
        bufferIndex = subBuffers.add(MemBuffer{														//Create a new buffer and save the buffer index
            //FIXME USE A NORMAL RaArray THAT ALLOCATES WITH MALLOC
            //! The map requires the chunk count and the max count. bufferSize is the count in bytes of the whole buffer, not the number of cells. The number of cells is (bufferSize / (uint32)vClass)
            //.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(max(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
            .cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(min(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
        });																							//^ Create in it 1 cell for custom count cells, or the maximum number of cells for fixed count cells
        MemBuffer& buffer = subBuffers[bufferIndex]; buffer.bufferIndex = bufferIndex;				//Set the buffer index of the created buffer
        //TODO set right like
        if(!buffer.memory) {
            buffer.memory = //Allocate new memory if the buffer has not already been allocated
                win10(_aligned_malloc((uint32)vClass ? lux::__pvt::bufferSize : vSize, LuxMemOffset);)
                linux( aligned_alloc( LuxMemOffset, (uint32)vClass ? lux::__pvt::bufferSize : vSize);)

            //TODO remove
            lux::ram::allocated += (uint32)vClass ? lux::__pvt::bufferSize : vSize;
            Main printf("allocated MBs: %d", allocated / 1000000);
        }


        cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{
            .cellSize = vSize,
            .bufferType = &buffers[typeIndex]
        })];
        this->address = (type*)(cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell)));	//<^ Create a new cell in the new buffer. Set its address
        cell->cellIndex = (uint32)vClass ? cellIndex : 0;											//Set its index. 0 for custom count cells
    }
}








template<class type> void lux::ram::ptr<type, alloc>::realloc(const uint64 vSize, CellClass vCellClass) {
    evaluateCellClass(vSize, vCellClass);

    if(!this->address) { [[unlikely]]						//If the pointer has not been allocated
        alloc_(vSize, vCellClass);					//Allocate it
        return;
    }
    else { 													//If it's allocated
        int64 d = vSize - size( );							//Calculate the difference in size between the current size and the new size
        if(d < 0) [[unlikely]] cell->cellSize = vSize;		//If the new size is smaller, change the cellSize variable and return
        else if(d > 0) { [[likely]]							//If it's larger
            if(vSize <= (int64)vCellClass) { [[likely]]			//But not larger than the maximum cell size
                cell->cellSize = vSize;								//Change the cellSize variable
            }
            else {												//If it's also larger than the cell
                ram::ptr<type, alloc> ptr_(vSize, vCellClass);		//Allocate a new pointer
                memcpy(ptr_, this->address, size( ));				//Copy the old data
                free();												//Free the old cell
                *this = ptr_;										//Overwrite the cell itself. This is necessary in order to keep the pointers updated
            }
        }
        else [[unlikely]] return;							//If it has the same size, do nothing
    }
}
