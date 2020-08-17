#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Core/Compute/CBuffers.h"






namespace lux::vmem{
	// |--------------------------------- VRAM -------------------------------|
	// |------- Buffer0 ------||------- Buffer1 ------||------- Buffer2 ------|
	// |-cell-||-cell-||-cell-|
	//A video memory cell
	//A cell is a fixed-size partition of memory inside an allocated GPU buffer
	//Allocate a cell with the lux::vmem::alloc function
	struct Cell {
		uint64 cellSize;			//Size of the cell in bytes
		void* address;				//Address of the cell. The same as you would get with malloc
		uint32 bufferTypeIndex;		//Type of buffer allocation
		uint32 bufferIndex;			//Index of the buffer where the cell is allocated
		uint32 cellIndex;			//Index of the cell in the buffer
		uint32 __256padding;
		//TODO AVX2 = operator
	};



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
		LUX_CELL_CLASS_AUTO = (uint32)-1,			//Choose a class large enough to contain the cell
	};
	const uint32 bufferSize = LUX_CELL_CLASS_L * 6;	//Size of each buffer. 50331648 B (~50MB)
	enum CellClassIndex : uint32{
		LUX_CELL_CLASS_INDEX_A = 0b000,
		LUX_CELL_CLASS_INDEX_B = 0b001,
		LUX_CELL_CLASS_INDEX_C = 0b010,
		LUX_CELL_CLASS_INDEX_D = 0b011,
		LUX_CELL_CLASS_INDEX_Q = 0b100,
		LUX_CELL_CLASS_INDEX_L = 0b101,
		LUX_CELL_CLASS_INDEX_0 = 0b110,
		LUX_CELL_CLASS_NUM 							//Number of LUX_CELL_CLASS values
	};


	//TODO check for alllocation number limit
	//TODO use multiple devices
	enum AllocType : uint32{
		LUX_ALLOC_TYPE_DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		LUX_ALLOC_TYPE_DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		LUX_ALLOC_TYPE_SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		LUX_ALLOC_TYPE_SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		LUX_ALLOC_TYPE_NUM							//Number of LUX_ALLOC_TYPE values
	};
	constexpr bool isUniform(const AllocType vAllocType) { return (vAllocType & 0b1); }
	constexpr bool isShared(const AllocType vAllocType) { return ((vAllocType >> 1) & 0b1); }
	//Returns an index based on the cell class
	constexpr uint32 getCellClassIndex(const CellClass vClass){
		switch(vClass){
			#define _case(n) case LUX_CELL_CLASS_##n: return LUX_CELL_CLASS_INDEX_##n;
			_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0)
		}
	}

	struct MemBuffer{
		VkBuffer buffer;				//The actual Vulkan buffer
		VkDeviceMemory memory;			//The memory of the buffer
		Map<bool, uint32> cells;		//The cells in the buffer
	};
	struct MemBufferType{
		CellClass cellClass;			//The class of the cells
		AllocType allocType;			//The buffer allocation type
		Map<MemBuffer, uint32> buffers;	//Buffers containing the cells
	};




	extern uint32 maxAlloc;						//The maximum number of allocated buffers. Depends on the gpu properties
	extern Array<MemBufferType> buffers;		//Allocated VRAM buffers
	static inline void init( ){
		//Set max allocation count and resize buffer types array
		maxAlloc = lux::core::dvc::compute.PD.properties.limits.maxMemoryAllocationCount;
		buffers.resize(LUX_CELL_CLASS_NUM * LUX_ALLOC_TYPE_NUM);

		//Init buffer types
		for(uint32 i = 0; i < LUX_CELL_CLASS_NUM; ++i){
			for(uint32 j = 0; j < LUX_ALLOC_TYPE_NUM; ++j){
				buffers[i * LUX_CELL_CLASS_NUM + j].cellClass = (CellClass)i;
				buffers[i * LUX_CELL_CLASS_NUM + j].allocType = (AllocType)j;
			}
		}
	}


	Cell alloc(const uint64 vSize, const CellClass vCellClass, const AllocType vAllocType);
	void* map(Cell& pCell);
	void free(Cell& pCell);

	//Generates the index of a buffer from the cell class and allocation type
	// 1 0 1 | 0 1
	// class | type
	constexpr uint32 genBufferTypeIndex(const CellClass vClass, const AllocType vAllocType){ return (getCellClassIndex(vClass) << 2) | vAllocType; }
	static inline CellClass getCellClass(const Cell& pCell){ return buffers[pCell.bufferTypeIndex].cellClass; }
	static inline uint32 getCellOffset(const Cell& pCell){ return getCellClass(pCell) * pCell.cellIndex; }
	static inline VkDeviceMemory getCellMemory(const Cell& pCell){ return buffers[pCell.bufferTypeIndex].buffers[pCell.bufferIndex].memory; }
}
