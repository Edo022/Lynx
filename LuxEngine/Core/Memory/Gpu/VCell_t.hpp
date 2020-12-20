#pragma once
#define LUX_H_VCELL_T
#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/__nmp_RaArray.hpp"







namespace lux{
	enum class AllocType : uint32 {
		DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		NUM							//Number of LUX_ALLOC_TYPE values
	};
    namespace rem{
        struct Buffer_t; inline void __unmap(Buffer_t* __buffer);
        struct Type_t;


        struct Cell_t {
            uint64 cellSize;					//Size of the cell in bytes
            Type_t* bufferType;			//Type of buffer allocation
            Buffer_t* buffer;					//Index of the buffer where the cell is allocated
            uint32 cellIndex;					//Index of the cell in the buffer

            void* map();
            inline void unmap(){ __unmap(buffer); }
        };
        struct Buffer_t {
            VkBuffer buffer;					//Vulkan buffer object
            VkDeviceMemory memory;				//Vulkan buffer memory
            // Map_NMP_S<Cell_t, uint32> cells;	//Cells in the buffer
            __nmp_RaArray<Cell_t, uint32, 32> cells;	//Cells in the buffer
            // inline void unmap(){ vkUnmapMemory(core::dvc::compute.LD, memory); }
        };
        inline void __unmap(Buffer_t* __buffer){ vkUnmapMemory(core::dvc::compute.LD, __buffer->memory); }

        struct Type_t {
            CellClass cellClass;				//Class of the cells
            lux::AllocType allocType;	        //Buffer allocation type
            // Map_NMP_S<Buffer_t, uint32> buffers;//Buffers containing the cells
            __nmp_RaArray<Buffer_t, uint32, 32> buffers;//Buffers containing the cells
        };
    }
}