#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Types/Containers/LuxMap.h"




namespace lux{
	// |--------------------------------- VRAM -------------------------------|
	// |------- Buffer0 ------||------- Buffer1 ------||------- Buffer2 ------|
	// |-cell-||-cell-||-cell-|
	//This struct defines a video memory cell
	//A cell is a fixed-size portion of memory inside an allocated buffer
	//Create a cell with the lux::vmem::alloc function
	struct VMemCell {
		uint64 size;			//Size of the cell in bytes
		void* address;			//Address of the cell. The same as you would get with malloc
		uint32 bufferTypeIndex;	//Index of the buffer type
		uint32 bufferIndex;		//Index of the buffer where the cell is allocated in

		uint64 __256padding;
		//TODO AVX2 = operator
	};


	namespace vmem{
		//Bytes to allocate for each cell
		//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets
		enum CellClass : uint32{
			LUX_CELL_CLASS_A = 32,						//32 bytes. The minimum size of a cell
			LUX_CELL_CLASS_B = LUX_CELL_CLASS_A * 16,	//16x LUX_CELL_CLASS_A. 512 B per cell (~0.5KB)
			LUX_CELL_CLASS_C = LUX_CELL_CLASS_A * 32,	//32x LUX_CELL_CLASS_A. 1024 B per cell (~1KB)
			LUX_CELL_CLASS_D = LUX_CELL_CLASS_A * 64,	//64x LUX_CELL_CLASS_A. 2048 B per cell (~2KB)
			LUX_CELL_CLASS_Q = LUX_CELL_CLASS_D * 64,	//64x LUX_CELL_CLASS_D. 131072 B per cell (~130KB)
			LUX_CELL_CLASS_L = LUX_CELL_CLASS_Q * 64,	//64x LUX_CELL_CLASS_Q. 8388608 B per cell (~8.4MB)
			LUX_CELL_CLASS_0 = 0,						//Dedicated buffer for cells larger than LUX_CELL_CLASS_L
			LUX_CELL_CLASS_AUTO = -1,					//Choose a class large enough to contain the cell
			LUX_CELL_CLASS_NUM							//The number of LUX_CELL_CLASS values
		};
		enum AllocType : uint32{
			LUX_ALLOC_TYPE_DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
			LUX_ALLOC_TYPE_DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
			LUX_ALLOC_TYPE_SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
			LUX_ALLOC_TYPE_SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		};
		constexpr bool isUniform(const AllocType vAllocType) { return (vAllocType & 0b1); }
		constexpr bool isShared(const AllocType vAllocType) { return ((vAllocType >> 1) & 0b1); }
		const uint32 bufferSize = 67108864;			//Size of each buffer. ~67MB
		//Returns an index based on the cell class
		constexpr uint32 getCellClassIndex(const CellClass vClass){
			switch(vClass){
				case LUX_CELL_CLASS_A:    return 0b0000;
				case LUX_CELL_CLASS_B:    return 0b0001;
				case LUX_CELL_CLASS_C:    return 0b0010;
				case LUX_CELL_CLASS_D:    return 0b0011;
				case LUX_CELL_CLASS_Q:    return 0b0100;
				case LUX_CELL_CLASS_L:    return 0b0101;
				case LUX_CELL_CLASS_0:    return 0b0111;
				//case LUX_CELL_CLASS_AUTO: return 0b1001;
			}
		}

		struct MemBuffer{
			CellClass cellClass;	//The class of the cells
			AllocType allocType;	//The buffer allocation type
		};




		extern uint32 maxAlloc;						//The maximum number of allocated buffers. Depends on the gpu properties
		extern Array<Map<MemBuffer>> buffers;		//Allocated VRAM buffers
		static inline void init( ){
			maxAlloc = lux::core::dvc::compute.PD.properties.limits.maxMemoryAllocationCount;
			buffers.resize(LUX_CELL_CLASS_NUM * 2/*isShared bit*/ * 2/*isUniform bit*/);
		}


		//This function allocates a video memory cell into a buffer
		//*   vSize      | the size of the cell
		//*   vCellClass | the class of the cell. This is the maximum size the cell can reach before it needs to be reallocated
		//Returns the allocated cell
		//e.g.
		//lux::VMemCell foo = lux::vmem::alloc(100, LUX_CELL_CLASS_B);
		VMemCell alloc(const uint64 vSize, const CellClass vCellClass, const AllocType vAllocType);

		//Generates the index of a buffer from the cell class and allocation type
		constexpr uint32 genBufferIndex(const CellClass vClass, const AllocType vAllocType){
			return (getCellClassIndex(vClass) << 2) | vAllocType;
		}

	}
}