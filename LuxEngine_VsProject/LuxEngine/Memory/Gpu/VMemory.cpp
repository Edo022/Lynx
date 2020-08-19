#include "LuxEngine/Memory/Gpu/VMemory.h"




namespace lux::rem{
	uint32 maxAlloc;
	Array<MemBufferType> buffers;

	#define _case(n) case CELL_CLASS_##n: return CELL_CLASS_INDEX_##n;
	static constexpr inline uint32 classIndexFromEnum(const CellClass vClass){ switch(vClass){ _case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0) default: return (uint32)-1; } }
	#define _case2(n) case CELL_CLASS_INDEX_##n: return CELL_CLASS_##n;
	static constexpr inline CellClass classEnumFromIndex(const uint32 vClass){ switch(vClass){ _case2(A) _case2(B) _case2(C) _case2(D) _case2(Q) _case2(L) _case2(0) default: return (CellClass)-1; } }







	void init( ){
		//Set max allocation count and resize buffer types array
		maxAlloc = lux::core::dvc::compute.PD.properties.limits.maxMemoryAllocationCount;
		buffers.resize(CELL_CLASS_NUM * ALLOC_TYPE_NUM);

		//Init buffer types
		uint32 index;
		for(uint32 i = 0; i < CELL_CLASS_NUM; ++i){
			for(uint32 j = 0; j < ALLOC_TYPE_NUM; ++j){
				index = (i << 2) | j;
				buffers[index].cellClass = (CellClass)classEnumFromIndex(i);
				buffers[index].allocType = (AllocType)j;
				buffers[index].buffers = Map<MemBuffer, uint32>(32, 4096); //32 buffers per chunk, max 4096 buffers (max allocation limit in GPUs)
			}
		}
	}




	//This function allocates a video memory cell into a buffer
	//*   vSize      | size of the cell
	//*   vCellClass | class of the cell. This is the maximum size the cell can reach before it needs to be reallocated
	//*   vAllocType | type of buffer where to allocate the cell
	//*       Cells allocated in shared memory are accessible from both CPU and GPU (cpu needs to map() the cell to use it)
	//*       Cells allocated in dedicated memory are only accessible from GPU
	//*       Uniform buffers are read only for the GPU. Useful when you need to pass small data to a shader
	//*       Storage buffers are larger and the GPU can write in it, bet they have worse performance
	//*   Returns    | the allocated Cell object
	//e.g.   lux::rem::Cell foo = lux::rem::alloc(100, lux::rem::CELL_CLASS_AUTO, lux::rem::ALLOC_TYPE_DEDICATED_STORAGE);
	Cell alloc(const uint64 vSize, CellClass vCellClass, const AllocType vAllocType){
		//TODO fix comments
		//Set cell class if CELL_CLASS_AUTO was used
		if(vCellClass == CELL_CLASS_AUTO) {
			vCellClass =
				(vSize <= CELL_CLASS_A) ? CELL_CLASS_A :
				(vSize <= CELL_CLASS_B) ? CELL_CLASS_B :
				(vSize <= CELL_CLASS_C) ? CELL_CLASS_C :
				(vSize <= CELL_CLASS_D) ? CELL_CLASS_D :
				(vSize <= CELL_CLASS_Q) ? CELL_CLASS_Q :
				(vSize <= CELL_CLASS_L) ? CELL_CLASS_L :
				CELL_CLASS_0;
		}



		uint32 typeIndex = (classIndexFromEnum(vCellClass) << 2) | vAllocType;		//Get buffer index from type and class
		Map<MemBuffer, uint32>& typeBuffers = (buffers[typeIndex].buffers);			//Get list of buffers where to search for a free cell
		//Cell_t cell{																	//Cell object
		//	.cellSize = vSize,
		//	.bufferType = &buffers[typeIndex]
		//};

		uint32 cellIndex;
		if(vCellClass){																//If the cell is a fixed size cell
			uint64 cellNum = bufferSize / vCellClass;									//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < typeBuffers.size( ); i++){							//Search for a suitable buffer
				if(typeBuffers.isValid(i) && (typeBuffers[i].cells.usedSize( ) < cellNum)) {//If a buffer is valid and it has a free cell
					Cell cell = &typeBuffers[i].cells[(cellIndex = typeBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] }))];
					cell->buffer = &typeBuffers[i];												//Set it as the cell's buffer
					cell->cellIndex = cellIndex;												//Add to it a new cell, assign the cell index
					return cell;																//And return the cell object
				}
			}
		}{																			//If there are no free buffers or the cell is a custom size cell
			//Create a new buffer with 1 cell for custom size cells, or the max number of cells for fixed size cells. Then set it as the cell's buffer
			MemBuffer& buffer = typeBuffers[typeBuffers.add(MemBuffer{ 0, 0, !vCellClass ? Map<Cell_t, uint32>(1, 1) : Map<Cell_t, uint32>(bufferSize / vCellClass, bufferSize / vCellClass) })];
			Cell cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->buffer = &buffer;
			cell->cellIndex = !vCellClass ? 0 : cellIndex;
			core::c::buffers::createBuffer(												//Set the cell index. Create a new vk buffer
				core::dvc::compute.LD, !vCellClass ? vSize : bufferSize,
				((isUniform(vAllocType) && (core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize)) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				isShared(vAllocType) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&buffer.buffer, &buffer.memory								//with the buffer's buffer and device memory
			);																			//And
			return cell;																//return the cell object
		}
		//TODO incorrect maxUniformBufferRange. It's UINT_MAX, for some reason
	}










	//Maps a lux::rem::Cell to a memory address in order to use it from the CPU
	//Returns the address of the cell as a void pointer
	//Only cells allocated in shared memory can be mapped
	//Always lux::vmem::unmap() cells when you don't need to access their data
	void* map(Cell pCell){
		void* data;
		vkMapMemory(core::dvc::compute.LD, pCell->buffer->memory, getCellOffset(pCell), pCell->bufferType->cellClass, 0, &data);
		return data;
	}



	//Frees a video memory cell
	void free(Cell pCell){
		//TODO destroy buffers from asyncrhonous garbage collector
		pCell->buffer->cells.remove(pCell->cellIndex);
		//if(pCell.buffer->cells.usedSize() == 0) pCell.bufferType->buffers.remove(pCell)
	}
}
