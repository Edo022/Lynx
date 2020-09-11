#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Core/ConsoleOutput.h"
//#include "LuxEngine/Core/Core.h"




namespace lux::ram{
	MemBufferType* buffers;
	uint32 allocated = 0;






	void init( ){
		buffers = (MemBufferType*)malloc(sizeof(MemBufferType) * (uint32)CellClassIndex::NUM * (uint32)AllocType::NUM);

		//Init buffer types
		for(uint32 i = 0; i < (uint32)CellClassIndex::NUM; ++i){
			buffers[i].cellClass = (CellClass)classEnumFromIndex((CellClassIndex)i);
			buffers[i].buffers = Map_NMP_S<MemBuffer, uint32>(32, 8192); //64 buffers per chunk, max 8192 buffers
		}
	}




	//This function allocates a memory cell or a pointer into a buffer
	//*   vSize                 | size of the cell
	//*   vClass                | class of the cell. This is the maximum size the cell can reach before it needs to be copied
	//*   vForceDedicatedBuffer | if true, the memory will be allocated in a new buffer instead of a fixed size cell
	//*   Returns               | the allocated Cell object
	//e.g. lux::ram::ptr<int> foo = lux::ram::alloc(100, lux::CellClass::AUTO);
	//e.g. same as int* foo = (int*)malloc(100);
	Cell alloc(const uint64 vSize, CellClass vClass, const bool vForceDedicatedBuffer){
		luxDebug(if(vClass != CellClass::AUTO && (uint32)vClass < vSize) param_error(vClass, "The cell class must be large enought to contain the cell. Use lux::CellClass::AUTO to automatically choose it"));
		luxDebug(if(vSize > 0xFFFFffff) param_error(vSize, "The cell size cannot exceed 0xFFFFFFFF bytes"));

		//Set cell class if CellClass::AUTO was used or set it to custom size buffer if vForceDedicatedBuffer is true
		if(vForceDedicatedBuffer) vClass = CellClass::CLASS_0;
		else if(vClass == CellClass::AUTO) {
			if(vSize <= (uint32)CellClass::CLASS_A) [[likely]] vClass = CellClass::CLASS_A;
			else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]] vClass = CellClass::CLASS_B;
			else if(vSize <= (uint32)CellClass::CLASS_C) [[unlikely]] vClass = CellClass::CLASS_C;
			else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]] vClass = CellClass::CLASS_D;
			else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] vClass = CellClass::CLASS_Q;
			else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] vClass = CellClass::CLASS_L;
			else [[likely]] vClass = CellClass::CLASS_0;
		}


		uint32 typeIndex = classIndexFromEnum(vClass);												//Get buffer index from type and class
		Map_NMP_S<MemBuffer, uint32>& subBuffers = (buffers[typeIndex].buffers);							//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vClass){																			//If the cell is a fixed size cell
			uint64 cellNum = bufferSize / (uint32)vClass;												//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < subBuffers.size( ); i++){												//Search for a suitable buffer
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {					//If a buffer is valid and it has a free cell
					cellIndex = subBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] });
					Cell cell = &subBuffers[i].cells[cellIndex];											//<^ Create a new cell in the buffer and set the its buffer, index and address
					cell->buffer = &subBuffers[i];
					cell->cellIndex = cellIndex;
					cell->address = (void*)((uint8*)(cell->buffer->memory) + getCellOffset(cell));
					return cell;
				}
			}
		}{																							//If there are no free buffers or the cell is a custom size cell
			uint32 bufferIndex, cellsNum = (uint32)vClass ? bufferSize / (uint32)vClass : 1;
			bufferIndex = subBuffers.add(MemBuffer{														//Create a new buffer and save the buffer index
				//! The map requires the chunk size and the max size. bufferSize is the size in bytes of the whole buffer, not the number of cells. The number of cells is (bufferSize / (uint32)vClass)
				.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(max(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
			});																							//^ Create in it 1 cell for custom size cells, or the maximum number of cells for fixed size cells
			MemBuffer& buffer = subBuffers[bufferIndex]; buffer.bufferIndex = bufferIndex;				//Set the buffer index of the created buffer
			if(!buffer.memory) {
				buffer.memory = _aligned_malloc((uint32)vClass ? bufferSize : vSize, 32);//Allocate new memory if the buffer has not already been allocated
				//TODO remove
				allocated += (uint32)vClass ? bufferSize : vSize;
				Main printf("allocated MBs: %d", allocated / 1000000);
			}


			Cell cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell));	//<^ Create a new cell in the new buffer. Set its address
			cell->cellIndex = (uint32)vClass ? cellIndex : 0;											//Set its index. 0 for custom size cells


			//system("pause");
			return cell;
		}
	}




	//TODO check new cell class
	void realloc(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass){
		if(!pCell->address) [[unlikely]] {
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
	void free(Cell_t* pCell){
		//TODO destroy buffers from asyncrhonous garbage collector
		pCell->buffer->cells.remove(pCell->cellIndex);
		if(pCell->buffer->cells.usedSize( ) == 0) {
			pCell->bufferType->buffers.remove(pCell->buffer->bufferIndex);
			//TODO free only if there is not enough memory
		}
	}
}

//TODO automatic string subclass if memory pool is not available
