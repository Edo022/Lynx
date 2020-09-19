#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Core/ConsoleOutput.h"
//#include "LuxEngine/Core/Core.h"



//TODO background cell preallocation


namespace lux::ram{
	MemBufferType* buffers{ buffers };
	uint32 allocated{ allocated };






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
	Cell_t* alloc__(const uint64 vSize, const CellClass vClass){
		uint32 typeIndex = classIndexFromEnum(vClass);												//Get buffer index from type and class
		Map_NMP_S<MemBuffer, uint32>& subBuffers = (buffers[typeIndex].buffers);							//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vClass){																			//If the cell is a fixed size cell
			uint64 cellNum = bufferSize / (uint32)vClass;												//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < subBuffers.size( ); i++){												//Search for a suitable buffer
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {					//If a buffer is valid and it has a free cell
					cellIndex = subBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] });
					Cell_t* cell = &subBuffers[i].cells[cellIndex];											//<^ Create a new cell in the buffer and set the its buffer, index and address
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
				//.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(max(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
				.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(min(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
			});																							//^ Create in it 1 cell for custom size cells, or the maximum number of cells for fixed size cells
			MemBuffer& buffer = subBuffers[bufferIndex]; buffer.bufferIndex = bufferIndex;				//Set the buffer index of the created buffer
			//TODO set right like
			//if(!buffer.memory) {
			if(true) {
				buffer.memory = _aligned_malloc((uint32)vClass ? bufferSize : vSize, 32);//Allocate new memory if the buffer has not already been allocated
				//TODO remove
				allocated += (uint32)vClass ? bufferSize : vSize;
				Main printf("allocated MBs: %d", allocated / 1000000);
			}


			Cell_t* cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell));	//<^ Create a new cell in the new buffer. Set its address
			cell->cellIndex = (uint32)vClass ? cellIndex : 0;											//Set its index. 0 for custom size cells


			//system("pause");
			return cell;
		}
	}


	template<class type> ptr<type> AllocVA(const uint64 vSize, const CellClass vClass, const type& pValue){
		ptr<type> ptr = ram::alloc(vSize, vClass);
		for(auto e : ptr) e = pValue;
		return ptr;
	}


	template<class type> ptr<type> AllocDA(const uint64 vSize, const CellClass vClass){
		ptr<type> ptr = ram::alloc(vSize, vClass);
		for(auto e : ptr) e = type( );
		return ptr;
	}





	void realloc(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass){
		//If the cell is not allocated, allocate it and return
		if(!pCell->address) [[unlikely]] {
			pCell = alloc(vSize, vCellClass);
			return;
		}
		//If the class doesn't need to be changed
		//(class specified but it's the same as before or class is auto) and (size is within the class minimum and maximum size)
		if((vCellClass == CellClass::AUTO && vSize < (uint32)pCell->bufferType->cellClass) || (vCellClass == pCell->bufferType->cellClass && vSize < (uint32)vCellClass)) [[likely]] {
			pCell->cellSize = vSize;
		}
		//else (if the class has to be changed)
		else if(vSize != pCell->cellSize) [[unlikely]] {
			Cell_t * cell = alloc(vSize, vCellClass);	//Allocate a new cell
			memcpy(cell, pCell, pCell->cellSize);		//Copy the old data in the new cell
			ram::free(pCell);							//Free the old memory
			*pCell = *cell;								//Update the cell (and all the pointers pointing to it)
		}
	}





	//Frees a memory cell
	void free(Cell_t* pCell){
		//TODO destroy buffers from asyncrhonous garbage collector
		pCell->buffer->cells.remove(pCell->cellIndex);
		if(pCell->buffer->cells.usedSize( ) == 0) {
			pCell->bufferType->buffers.remove(pCell->buffer->bufferIndex);
			//TODO free only if there is not enough memory
		}
	}
}

