#include "LuxEngine/Memory/Gpu/VMemory.h"
#include "LuxEngine/Core/Core.h"




namespace lux::rem{
	uint32 maxAlloc{ maxAlloc };
	MemBufferType* buffers{ buffers };






	void init( ){
		//Set max allocation count and resize buffer types array
		maxAlloc = lux::core::dvc::compute.PD.properties.limits.maxMemoryAllocationCount;
		buffers = (MemBufferType*)malloc(sizeof(MemBufferType) * (uint32)CellClassIndex::NUM * (uint32)AllocType::NUM);

		//Init buffer types
		uint32 index;
		for(uint32 i = 0; i < (uint32)CellClassIndex::NUM; ++i){
			for(uint32 j = 0; j < (uint32)AllocType::NUM; ++j){
				index = (i << 2) | j;
				buffers[index].cellClass = (CellClass)classEnumFromIndex((CellClassIndex)i);
				buffers[index].allocType = (AllocType)j;
				buffers[index].buffers = Map_NMP_S<MemBuffer, uint32>(32, 4096); //32 buffers per chunk, max 4096 buffers (max allocation limit in GPUs)
			}
		}
	}




	//This function allocates a video memory cell into a buffer
	//*   vSize      | count of the cell
	//*   vCellClass | class of the cell. This is the maximum count the cell can reach before it needs to be reallocated
	//*   vAllocType | type of buffer where to allocate the cell
	//*       Cells allocated in shared memory are accessible from both CPU and GPU (cpu needs to map() the cell to use it)
	//*       Cells allocated in dedicated memory are only accessible from GPU
	//*       Uniform buffers are read only for the GPU. Useful when you need to pass small data to a shader
	//*       Storage buffers are larger and the GPU can write in it, bet they have worse performance
	//*   Returns    | the allocated Cell object
	//e.g.   lux::rem::Cell foo = lux::rem::allocBck(100, lux::CellClass::AUTO, lux::AllocType::DEDICATED_STORAGE);
	Cell allocBck(const uint64 vSize, CellClass vCellClass, const AllocType vAllocType){
		luxDebug(if(vCellClass != CellClass::AUTO && (uint32)vCellClass < vSize) param_error(vCellClass, "The cell class must be large enought to contain the cell. Use lux::CellClass::AUTO to automatically choose it"));
		luxDebug(if(vSize > 0xFFFFffff) param_error(vSize, "The cell size cannot exceed 0xFFFFFFFF bytes"));
		luxDebug(if(vAllocType >= AllocType::NUM || (int32)vAllocType < 0) param_error(vAllocType, "The allocation type can only be 'DEDICATED_STORAGE', 'DEDICATED_UNIFORM', 'SHARED_STORAGE' or 'SHARED_UNIFORM'"));


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

		//TODO fix like ram cells

		uint32 typeIndex = (classIndexFromEnum(vCellClass) << 2) | (uint32)vAllocType;		//Get buffer index from type and class
		Map_NMP_S<MemBuffer, uint32>& subBuffers = (buffers[typeIndex].buffers);					//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vCellClass){																//If the cell is a fixed count cell
			uint64 cellNum = bufferSize / (uint32)vCellClass;									//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < subBuffers.size( ); i++){										//Search for a suitable buffer
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {			//If a buffer is valid and it has a free cell
					Cell cell = &subBuffers[i].cells[(cellIndex = subBuffers[i].cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] }))];
					cell->buffer = &subBuffers[i];														//Set it as the cell's buffer
					cell->cellIndex = cellIndex;														//Add a new cell to it and set the cell index
					return cell;																		//Return the cell object
				}
			}
			//TODO fix
			//TODO like RAM cells
		}{																					//If there are no free buffers or the cell is a custom count cell
			//Create a new buffer with 1 cell for custom count cells, or the max number of cells for fixed count cells. Then set it as the cell's buffer
			MemBuffer& buffer = subBuffers[subBuffers.add(MemBuffer{ 0, 0, (uint32)vCellClass ? Map_NMP_S<Cell_t, uint32>(bufferSize / (uint32)vCellClass, bufferSize / (uint32)vCellClass) : Map_NMP_S<Cell_t, uint32>(1, 1) })];
			Cell cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{ .cellSize = vSize, .bufferType = &buffers[typeIndex] })];
			cell->buffer = &buffer;																//Create a new buffer and set it as the cell's buffer
			cell->cellIndex = (uint32)vCellClass ? cellIndex : 0;								//Add a new cell and set the cell index
			core::c::buffers::createBuffer(														//Create a new vk buffer
				core::dvc::compute.LD,(uint32)vCellClass ? bufferSize : vSize,					//#LLID STRT 0000 isUniform()
				((((uint32)vAllocType & 0b1) && (core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize)) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				((uint32)vAllocType >> 1) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&buffer.buffer, &buffer.memory
			);
			return cell;
		}
		//TODO incorrect maxUniformBufferRange. It's UINT_MAX, for some reason
	}










	//Maps a lux::rem::Cell to a memory address in order to use it from the CPU
	//Returns the address of the cell as a void pointer
	//Only cells allocated in shared memory can be mapped
	//Always unmap() cells when you don't need to access their data
	void* Cell_t::map(){
		void* data;
		vkMapMemory(core::dvc::compute.LD, buffer->memory, getCellOffset(this), (uint32)bufferType->cellClass, 0, &data);
		return data;
	}



	//Frees a video memory cell
	void free(Cell pCell){
		//TODO destroy buffers from asyncrhonous garbage collector
		pCell->buffer->cells.remove(pCell->cellIndex);
		//if(pCell.buffer->cells.usedCount() == 0) pCell.bufferType->buffers.remove(pCell)
	}
}
