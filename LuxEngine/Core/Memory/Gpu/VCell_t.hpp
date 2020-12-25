#pragma once
#define LUX_H_VCELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/RaArrayC.hpp"







namespace lux{
	enum class AllocType : uint32 {
		DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		NUM							//Number of LUX_ALLOC_TYPE values
	};
    namespace rem{
        // struct Buffer_t; inline void __unmap(Buffer_t* __buffer);
        struct VType_t;
        struct VCell_t;

        extern VType_t types[];
        extern RaArray<VCell_t> cells;


        struct VCell_t {
            uint16 typeIndex;		    //INDEX of the buffer type
            uint32 cellIndex;		    //Index of the cell in the cells array
            uint32 localIndex;          //Index of the cell in the type allocations
            uint64 cellSize;		    //Size of the cell in bytes
            // Buffer_t* buffer;					//Index of the buffer where the cell is allocated

            // void* map();
            // inline void unmap(){ __unmap(buffer); }
        };
        // struct Buffer_t {
        //     VkBuffer buffer;					//Vulkan buffer object
        //     VkDeviceMemory memory;				//Vulkan buffer memory
        //     // Map_NMP_S<Cell_t, uint32> cells;	//Cells in the buffer
        //     __nmp_RaArray<Cell_t, uint32, 32> cells;	//Cells in the buffer
        //     // inline void unmap(){ vkUnmapMemory(core::dvc::compute.LD, memory); }
        // };
        // inline void __unmap(Buffer_t* __buffer){ vkUnmapMemory(core::dvc::compute.LD, __buffer->memory); }

        struct VType_t {
            CellClass cellClass;		//Class of the cells
            lux::AllocType allocType;   //Buffer allocation type

            VkBuffer* buffer;		    //Vulkan buffer object
            VkDeviceMemory* memory;		//Vulkan buffer memory
            // Map_NMP_S<Buffer_t, uint32> buffers;//Buffers containing the cells
            // __nmp_RaArray<Buffer_t, uint32, 32> buffers;//Buffers containing the cells

            RaArrayC<bool> cells;       //TODO use optimized uint64 array
            uint32 cellsPerBuffer;      //Number of cells in each buffer
        };
    }
}