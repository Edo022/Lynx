#include "LuxEngine/Memory/Gpu/VMemory.h"



typedef uint64 uint31;
namespace lux::vmem{
	uint32 maxAlloc;
	Array<Map<MemBuffer>> buffers;


	VMemCell alloc(const uint64 vSize, const CellClass vCellClass, const AllocType vAllocType){
		uint31 index;

		uint32 bufferIndex = genBufferIndex(vCellClass, vAllocType);

		return VMemCell{
			.
		}
	}
}