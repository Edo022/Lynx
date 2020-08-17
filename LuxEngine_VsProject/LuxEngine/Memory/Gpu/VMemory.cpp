#include "LuxEngine/Memory/Gpu/VMemory.h"




namespace lux::vmem{
	uint32 maxAlloc;
	Array<MemBufferType> buffers;




	//This function allocates a video memory cell into a buffer
	//*   vSize      | the size of the cell
	//*   vCellClass | the class of the cell. This is the maximum size the cell can reach before it needs to be reallocated
	//Returns the allocated cell
	//e.g.
	//lux::VMemCell foo = lux::vmem::alloc(100, LUX_CELL_CLASS_B);
	VMemCell alloc(const uint64 vSize, CellClass vCellClass, const AllocType vAllocType){
		//TODO special case class 0
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

		//Search for a free buffer. Create a new one if there are none
		uint32 bufferTypeIndex = genBufferTypeIndex(vCellClass, vAllocType);
		Map<MemBuffer, uint32>& typeBuffers = (buffers[bufferTypeIndex].buffers);	//List of buffers where to search for a free buffer
		uint64 cellNum = bufferSize / buffers[bufferTypeIndex].cellClass;			//Number of cells in each buffer
		VMemCell cell{
			.cellSize = vSize,																//With the specified size,
			.address = nullptr,																//No address,
			.allocType = vAllocType															//And the buffer's allocation type
		};

		//Search for a buffer with the same properties and a free cell. If there is one, allocate the cell in it and return
		for(int32 i = 0; i < typeBuffers.size( ); i++){
			//TODO init memory and buffer members
			if(typeBuffers.isValid(i) && (typeBuffers[i].cells.usedSize( ) < cellNum)) {
				cell.bufferIndex = i;
				cell.cellIndex = typeBuffers[i].cells.add(true);
				return cell;
			}
		}

		{ //If there are none, create a new buffer and allocate the cell in it
			//TODO wrong maxUniformBufferRange. It's UINT_MAX, for some reason
			//Buffer type: Uniform buffer if the data fits in the max uniform buffer size and it's a read only buffer, Storage buffer otherwise
			//Memory type: Shared memory if it needs to be accessed by the CPU, local GPU memory if not
			uint32 bufferIndex = (cell.bufferIndex = typeBuffers.add(MemBuffer{ 0, 0, { } }));
			cell.cellIndex = typeBuffers[bufferIndex].cells.add(true);
			core::c::buffers::createBuffer(
				core::dvc::compute.LD, vSize,
				((isUniform(vAllocType) && (core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize)) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				(isShared(vAllocType)) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&typeBuffers[bufferIndex].buffer, &typeBuffers[bufferIndex].memory
			);
			return cell;
		}
	}
}
