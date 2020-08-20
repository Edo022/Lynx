#include "LuxEngine/Memory/Ram/Memory.h"




namespace lux::ram{
	Array<MemBufferType> buffers;






	void init( ){
		buffers.resize((uint32)CellClassIndex::NUM * (uint32)AllocType::NUM);

		//Init buffer types
		for(uint32 i = 0; i < (uint32)CellClassIndex::NUM; ++i){
			buffers[i].cellClass = (CellClass)classEnumFromIndex((CellClassIndex)i);
			buffers[i].buffers = Map<MemBuffer, uint32>(32, 8192); //64 buffers per chunk, max 8192 buffers
		}
	}




	//This function allocates a memory cell into a buffer
	//*   vSize      | size of the cell
	//*   vCellClass | class of the cell. This is the maximum size the cell can reach before it needs to be reallocated
	//*   Returns    | the allocated Cell object
	//e.g.   lux::ram::Cell foo = lux::ram::alloc(100, lux::CellClass::AUTO);
	Cell alloc(const uint64 vSize, CellClass vCellClass){
		//TODO fix comments
		//Set cell class if CellClass::AUTO was used
		if(vCellClass == CellClass::AUTO) {
			vCellClass =
				(vSize <= (uint32)CellClass::CLASS_A) ? CellClass::CLASS_A :
				(vSize <= (uint32)CellClass::CLASS_B) ? CellClass::CLASS_B :
				(vSize <= (uint32)CellClass::CLASS_C) ? CellClass::CLASS_C :
				(vSize <= (uint32)CellClass::CLASS_D) ? CellClass::CLASS_D :
				(vSize <= (uint32)CellClass::CLASS_Q) ? CellClass::CLASS_Q :
				(vSize <= (uint32)CellClass::CLASS_L) ? CellClass::CLASS_L :
				CellClass::CLASS_0;
		}



		uint32 typeIndex = classIndexFromEnum(vCellClass);							//Get buffer index from type and class
		Map<MemBuffer, uint32>& typeBuffers = (buffers[typeIndex].buffers);			//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vCellClass){														//If the cell is a fixed size cell
			uint64 cellNum = bufferSize / (uint32)vCellClass;							//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < typeBuffers.size( ); i++){							//Search for a suitable buffer
				if(typeBuffers.isValid(i) && (typeBuffers[i].cells.usedSize( ) < cellNum)) {//If a buffer is valid and it has a free cell
					Cell cell = &typeBuffers[i].cells[(cellIndex = typeBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] }))];
					cell->address = (void*)((uint8*)(cell->buffer = &typeBuffers[i])->memory + getCellOffset(cell));
					cell->cellIndex = cellIndex;												//Add to it a new cell, assign the cell index
					return cell;																//And return the cell object
				}
			}
		}{																			//If there are no free buffers or the cell is a custom size cell
			//Create a new buffer with 1 cell for custom size cells, or the max number of cells for fixed size cells. Then set it as the cell's buffer
			MemBuffer& buffer = typeBuffers[typeBuffers.add(MemBuffer{ _aligned_malloc((uint32)vCellClass ? bufferSize : vSize, 32), (uint32)vCellClass ? Map<Cell_t, uint32>(bufferSize / (uint32)vCellClass, bufferSize / (uint32)vCellClass) : Map<Cell_t, uint32>(1, 1) })];
			Cell cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell));
			cell->cellIndex = (uint32)vCellClass ? cellIndex : 0;
			return cell;																//return the cell object
		}
		//TODO incorrect maxUniformBufferRange. It's UINT_MAX, for some reason
	}








	//Frees a video memory cell
	void free(Cell pCell){
		//TODO destroy buffers from asyncrhonous garbage collector
		pCell->buffer->cells.remove(pCell->cellIndex);
		//if(pCell.buffer->cells.usedSize() == 0) pCell.bufferType->buffers.remove(pCell)
	}
}
