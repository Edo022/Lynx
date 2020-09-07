#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Core/ConsoleOutput.h"
//#include "LuxEngine/Core/Core.h"



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




	//This function allocates a memory cell or a pointer into a buffer
	//*   vSize      | size of the cell
	//*   vCellClass | class of the cell. This is the maximum size the cell can reach before it needs to be reallocated
	//*   Returns    | the allocated Cell object
	//e.g.   lux::ram::ptr<int> foo = lux::ram::alloc(100, lux::CellClass::AUTO);
	//e.g.   same as int* foo = (int*)malloc(100);
	Cell alloc(const uint64 vSize, CellClass vCellClass){
		luxDebug(if(vCellClass != CellClass::AUTO && (uint32)vCellClass < vSize) param_error(vCellClass, "The cell class must be large enought to contain the cell. Use lux::CellClass::AUTO to automatically choose it"));
		luxDebug(if(vSize > 0xFFFFffff) param_error(vSize, "The cell size cannot exceed 0xFFFFFFFF bytes"));

		//TODO fix comments
		//Set cell class if CellClass::AUTO was used
		if(vCellClass == CellClass::AUTO) {
			if(vSize <= (uint32)CellClass::CLASS_A) [[likely]] vCellClass = CellClass::CLASS_A;
			else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]] vCellClass = CellClass::CLASS_B;
			else if(vSize <= (uint32)CellClass::CLASS_C) [[unlikely]] vCellClass = CellClass::CLASS_C;
			else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]] vCellClass = CellClass::CLASS_D;
			else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] vCellClass = CellClass::CLASS_Q;
			else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] vCellClass = CellClass::CLASS_L;
			else [[likely]] vCellClass = CellClass::CLASS_0;
		}



		uint32 typeIndex = classIndexFromEnum(vCellClass);							//Get buffer index from type and class
		Map<MemBuffer, uint32>& subBuffers = (buffers[typeIndex].buffers);			//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vCellClass){														//If the cell is a fixed size cell
			uint64 cellNum = bufferSize / (uint32)vCellClass;							//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < subBuffers.size( ); i++){							//Search for a suitable buffer
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {//If a buffer is valid and it has a free cell
					//Cell cell = &subBuffers[i].cells[(cellIndex = subBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] }))];
						cellIndex = subBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] });
						Cell cell = &subBuffers[i].cells[cellIndex];
						cell->buffer = &subBuffers[i];
					//cell->address = (void*)((uint8*)(cell->buffer = &subBuffers[i])->memory + getCellOffset(cell));
						cell->cellIndex = cellIndex;												//Add to it a new cell, assign the cell index
						cell->address = (void*)((uint8*)(cell->buffer->memory) + getCellOffset(cell));
					return cell;																//And return the cell object
				}
			}
		}{																			//If there are no free buffers or the cell is a custom size cell
			//Create a new buffer with 1 cell for custom size cells, or the max number of cells for fixed size cells. Then set it as the cell's buffer
			uint32 bufferIndex;

			//MemBuffer& buffer = subBuffers[bufferIndex = subBuffers.add(MemBuffer{ .memory = _aligned_malloc((uint32)vCellClass ? bufferSize : vSize, 32), .cells = (uint32)vCellClass ? Map<Cell_t, uint32>(bufferSize / (uint32)vCellClass, bufferSize / (uint32)vCellClass) : Map<Cell_t, uint32>(1, 1), .bufferIndex = (uint32)-1})];
			//TODO remove debug junk
			int __h = (
				(uint32)vCellClass ? (sizeof(lux::ram::Cell_t) * (bufferSize / (uint32)vCellClass)) : 32 +	//tracker map chunk size +
				((uint32)vCellClass ? bufferSize : vSize, 32))												//alloc size
				/ (1000 * 1000);																			//to MBs


			// - too many byffers are created
			bufferIndex = subBuffers.add(
				MemBuffer{
					.memory = _aligned_malloc((uint32)vCellClass ? bufferSize : vSize, 32),
					.cells = (uint32)vCellClass ? Map<Cell_t, uint32>(bufferSize / (uint32)vCellClass, bufferSize) : Map<Cell_t, uint32>(1, 1),
					.bufferIndex = (uint32)-1
				}
			);
			//^ bufferIndex does not increase and that's ok
			MemBuffer& buffer = subBuffers[bufferIndex];

			buffer.bufferIndex = bufferIndex;
			Cell cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell));
			cell->cellIndex = (uint32)vCellClass ? cellIndex : 0;
			return cell;																//return the cell object
		}
	}




	//TODO check new cell class
	void realloc(Cell& const pCell, const uint64 vSize, const CellClass vCellClass){
		if(!pCell->address) {
			pCell = alloc(vSize, vCellClass);
			return;
		}
		if((vCellClass == CellClass::AUTO && vSize < (uint32)pCell->bufferType->cellClass) || (vCellClass == pCell->bufferType->cellClass && vSize < (uint32)vCellClass)) [[likely]] pCell->cellSize = vSize;
		else if(vSize != pCell->cellSize) [[unlikely]] {
			ram::free(pCell);
			Cell cell = alloc(vSize, vCellClass);
			pCell->address = cell->address; pCell->buffer = cell->buffer; pCell->cellSize = cell->cellSize; pCell->bufferType = cell->bufferType; pCell->cellIndex = cell->cellIndex;
		}
	}





	//Frees a video memory cell
	void free(Cell pCell){
		//TODO destroy buffers from asyncrhonous garbage collector
		pCell->buffer->cells.remove(pCell->cellIndex);
		if(pCell->buffer->cells.usedSize() == 0) pCell->bufferType->buffers.remove(pCell->buffer->bufferIndex);
	}
}
