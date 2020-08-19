#include "LuxEngine/Memory/Gpu/VMemory.h"




namespace lux::rem{
	uint32 maxAlloc;
	Array<MemBufferType> buffers;







	//This function allocates a video memory cell into a buffer
	//*   vSize      | the size of the cell
	//*   vCellClass | the class of the cell. This is the maximum size the cell can reach before it needs to be reallocated
	//Returns the allocated cell
	//e.g.
	//lux::vramCell foo = lux::rem::alloc(100, LUX_CELL_CLASS_B);
	Cell alloc(const uint64 vSize, CellClass vCellClass, const AllocType vAllocType){
		//Set cell class if LUX_CELL_CLASS_AUTO was used
		if(vCellClass == LUX_CELL_CLASS_AUTO) {
			vCellClass =
				(vSize <= LUX_CELL_CLASS_A) ? LUX_CELL_CLASS_A :
				(vSize <= LUX_CELL_CLASS_B) ? LUX_CELL_CLASS_B :
				(vSize <= LUX_CELL_CLASS_C) ? LUX_CELL_CLASS_C :
				(vSize <= LUX_CELL_CLASS_D) ? LUX_CELL_CLASS_D :
				(vSize <= LUX_CELL_CLASS_Q) ? LUX_CELL_CLASS_Q :
				(vSize <= LUX_CELL_CLASS_L) ? LUX_CELL_CLASS_L :
				LUX_CELL_CLASS_0;
		}



		// 1 0 1 | 0 1
		// class | type
		uint32 typeIndex = (getCellClassIndex(vCellClass) << 2) | vAllocType;		//Get buffer index from type and class
		Map<MemBuffer, uint32>& typeBuffers = (buffers[typeIndex].buffers);			//Get list of buffers where to search for a free cell
		Cell cell{																	//Cell object
			.cellSize = vSize,
			.bufferType = &buffers[typeIndex]
		};

		if(vCellClass){											//If the cell is a fixed size cell
			uint64 cellNum = bufferSize / buffers[typeIndex].cellClass;					//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < typeBuffers.size( ); i++){							//Search for a suitable buffer
				if(typeBuffers.isValid(i) && (typeBuffers[i].cells.usedSize( ) < cellNum)) {//If a buffer is valid and it has a free cell
					cell.buffer = &typeBuffers[i];												//Set it as the cell's buffet
					cell.cellIndex = typeBuffers[i].cells.add(true);							//Add to it a new cell, assign the cell index
					return cell;																//And return the cell object
				}
			}
		}{																			//If there are no free buffers or the cell is a custom size cell
			//Create a new buffer with 1 cell for custom size cells, or the max number of cells for fixed size cells. Then set it as the cell's buffer
			cell.buffer = &typeBuffers[typeBuffers.add(MemBuffer{ 0, 0, (!vCellClass) ? Map<bool, uint32>(1, 1) : Map<bool, uint32>(bufferSize / vCellClass, bufferSize / vCellClass) })];
			cell.cellIndex = (!vCellClass) ? 0 : cell.buffer->cells.add(true);
			core::c::buffers::createBuffer(												//Set the cell index. Create a new vk buffer
				core::dvc::compute.LD, (!vCellClass) ? vSize : bufferSize,
				((isUniform(vAllocType) && (core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize)) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				(isShared(vAllocType)) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&cell.buffer->buffer, &cell.buffer->memory								//with the buffer's buffer and device memory
			);																			//And
			return cell;																//return the cell object
		}
		//TODO wrong maxUniformBufferRange. It's UINT_MAX, for some reason
	}










	//Returns the address of a cell
	//Only cells allocated in shared memory can be mapped
	//Always lux::vmem::unmap() cells when  you don't need to access their data
	void* map(Cell& pCell){
		void* data;
		vkMapMemory(core::dvc::compute.LD, pCell.buffer->memory, getCellOffset(pCell), pCell.bufferType->cellClass, 0, &data);
		return data;
	}



	//TODO add free function
	//Frees a video memory cell
	void free(Cell& pCell){

	}
}
