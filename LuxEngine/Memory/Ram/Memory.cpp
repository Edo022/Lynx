#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Core/ConsoleOutput.h"
#include <cstring>
#include "LuxEngine/Core/LuxAutoInit.hpp"
//#include "LuxEngine/Core/Core.h"



//TODO background cell preallocation
//TODO REMOVE EXPLICIT INITIALIZATION OF ENGINE NAMESPACES

namespace lux::ram{
	// MemBufferType* buffers;
	// uint32 allocated;


	// AutoInit(LUX_H_MEMORY){
	// 	buffers = (MemBufferType*)malloc(sizeof(MemBufferType) * (uint32)CellClassIndex::NUM * (uint32)AllocType::NUM);
	// 	//Init buffer types
	// 	for(uint32 i = 0; i < (uint32)CellClassIndex::NUM; ++i){
	// 		buffers[i].cellClass = (CellClass)classEnumFromIndex((CellClassIndex)i);
	// 		//TODO choose number of buffers based on the system memory
	// 		buffers[i].buffers = Map_NMP_S<MemBuffer, uint32>(32, 8192); //64 buffers per chunk, max 8192 buffers
	// 	}
	// }





	void init( ){
		// buffers = (MemBufferType*)malloc(sizeof(MemBufferType) * (uint32)CellClassIndex::NUM * (uint32)AllocType::NUM);

		// //Init buffer types
		// for(uint32 i = 0; i < (uint32)CellClassIndex::NUM; ++i){
		// 	buffers[i].cellClass = (CellClass)classEnumFromIndex((CellClassIndex)i);
		// 	//TODO choose number of buffers based on the system memory
		// 	buffers[i].buffers = Map_NMP_S<MemBuffer, uint32>(32, 8192); //64 buffers per chunk, max 8192 buffers
		// }
	}






	//TODO use offset as constant instead of literal
	void evaluateCellClass(const uint64 vSize, CellClass& pClass) {
		//Check AT_LEAST values. Set class to AUTO if it's too small. If it fits, assign it the normal class value
		if(pClass != CellClass::AUTO && (uint32)pClass % LuxMemOffset == 1) {
			if((uint32)pClass < vSize) pClass = CellClass::AUTO;
			else pClass = (CellClass)((uint64)pClass - 1);
		}
		//Choose cell class if it's AUTO
		if(pClass == CellClass::AUTO) { [[likely]]
			/**/ if(vSize <= (uint32)CellClass::CLASS_A) [[likely]] pClass = CellClass::CLASS_A;
			else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]] pClass = CellClass::CLASS_B;
			else if(vSize <= (uint32)CellClass::CLASS_C) [[likely]] pClass = CellClass::CLASS_C;
			else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]] pClass = CellClass::CLASS_D;
			else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] pClass = CellClass::CLASS_Q;
			else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] pClass = CellClass::CLASS_L;
			else										 pClass = CellClass::CLASS_0;
		}

		param_error_2(vSize > 0xFFFFffff, vSize, "Cell size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", vSize);
		param_error_2((uint32)pClass < vSize, pClass,
			"Requested %lu-bytes class for %llu-bytes allocation. The cell class must be large enought to contain the cell. %s",
			(uint32)pClass, vSize, "Use lux::CellClass::AUTO to automatically choose it");
	}









	//TODO CHECK MEMORY FULL
	//This function allocates a memory cell or a pointer into a buffer
	//*   vSize                 | count of the cell
	//*   vClass                | class of the cell. This is the maximum count the cell can reach before it needs to be copied
	//*   vForceDedicatedBuffer | if true, the memory will be allocated in a new buffer instead of a fixed count cell
	//*   Returns               | the allocated Cell object
	//e.g. lux::ram::ptr<int> foo = lux::ram::allocBck(100, lux::CellClass::AUTO);
	//e.g. same as int* foo = (int*)malloc(100);
	Cell_t* alloc_internal(const uint64 vSize, const CellClass vClass){
		uint32 typeIndex = classIndexFromEnum(vClass);												//Get buffer index from type and class
		Map_NMP_S<MemBuffer, uint32>& subBuffers = (buffers[typeIndex].buffers);							//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vClass){																			//If the cell is a fixed count cell
			uint64 cellNum = bufferSize / (uint32)vClass;												//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < subBuffers.size( ); i++){												//Search for a suitable buffer
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {					//If a buffer is valid and it has a free cell
					cellIndex = subBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] });
					Cell_t* cell = &subBuffers[i].cells[cellIndex];											//<^ Create a new cell in the buffer and set its buffer, index and address
					cell->buffer = &subBuffers[i];
					cell->cellIndex = cellIndex;
					cell->address = (void*)((uint8*)(cell->buffer->memory) + getCellOffset(cell));
					return cell;
				}
			}
		}{																							//If there are no free buffers or the cell is a custom count cell
			uint32 bufferIndex, cellsNum = (uint32)vClass ? bufferSize / (uint32)vClass : 1;
			bufferIndex = subBuffers.add(MemBuffer{														//Create a new buffer and save the buffer index
				//! The map requires the chunk count and the max count. bufferSize is the count in bytes of the whole buffer, not the number of cells. The number of cells is (bufferSize / (uint32)vClass)
				//.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(max(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
				.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(min(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
			});																							//^ Create in it 1 cell for custom count cells, or the maximum number of cells for fixed count cells
			MemBuffer& buffer = subBuffers[bufferIndex]; buffer.bufferIndex = bufferIndex;				//Set the buffer index of the created buffer
			//TODO set right like
			if(!buffer.memory) {
			//if(true) {
				#ifdef _WIN64
				buffer.memory = _aligned_malloc((uint32)vClass ? bufferSize : vSize, LuxMemOffset);//Allocate new memory if the buffer has not already been allocated
				#elif defined __linux__
				buffer.memory = aligned_alloc(LuxMemOffset, (uint32)vClass ? bufferSize : vSize);//Allocate new memory if the buffer has not already been allocated
				#endif

				//TODO remove
				allocated += (uint32)vClass ? bufferSize : vSize;
				Main printf("allocated MBs: %d", allocated / 1000000);
			}


			Cell_t* cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell));	//<^ Create a new cell in the new buffer. Set its address
			cell->cellIndex = (uint32)vClass ? cellIndex : 0;											//Set its index. 0 for custom count cells


			//system("pause");
			return cell;
		}
	}


	// template<class type> ptr<type> allocArr(const uint64 vSize, const CellClass vClass, const type& pValue){
	// 	ptr<type> ptr = ram::allocBck(vSize, vClass);
	// 	for(auto e : ptr) e = pValue;
	// 	return ptr;
	// }


	// template<class type> ptr<type> allocArr(const uint64 vSize, const CellClass vClass){
	// 	ptr<type> ptr = ram::allocBck(vSize, vClass);
	// 	for(auto e : ptr) e = type( );
	// 	return ptr;
	// }





	void reallocBck(Cell_t* pCell, const uint64 vSize, const CellClass vCellClass){
		//FIXME pCell can be nullptr. probably.
		//FIXME it's the same as when pCell->address is nullpt
		//If the cell is not allocated, allocate it and return
		// if(!pCell->address) { [[unlikely]]
		if(!pCell || !pCell->address) { [[unlikely]]
			pCell = allocBck(vSize, vCellClass);
			return;
		}
		//If the class doesn't need to be changed
		//(class specified but it's the same as before or class is auto) and (count is within the class minimum and maximum count)
		if((vCellClass == CellClass::AUTO && vSize < (uint32)pCell->bufferType->cellClass) || (vCellClass == pCell->bufferType->cellClass && vSize < (uint32)vCellClass)) {
			[[likely]];
			pCell->cellSize = vSize;
		}
		//else (if the class has to be changed)
		else if(vSize != pCell->cellSize) {
			Cell_t * cell = allocBck(vSize, vCellClass);	//Allocate a new cell
			//BUG memcpy copies the data in the cells structure and not the address they are pointing to
			//FIXME wtf is this? cells don't have an implicit pointer conversion
			// memcpy(cell, pCell, pCell->cellSize);		//Copy the old data in the new cell
			memcpy(cell->address, pCell->address, pCell->cellSize);		//Copy the old data in the new cell
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

