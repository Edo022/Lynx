#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"







namespace lux{
	enum class AllocType : uint32 {
		DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		NUM							//Number of LUX_ALLOC_TYPE values
	};
    namespace rem{
        struct MemBuffer;
        struct MemBufferType;
        struct Cell_t {
            uint64 cellSize;					//Size of the cell in bytes
            MemBufferType* bufferType;			//Type of buffer allocation
            MemBuffer* buffer;					//Index of the buffer where the cell is allocated
            uint32 cellIndex;					//Index of the cell in the buffer

            void* map();
            inline void unmap();
        };

        struct MemBuffer {
            VkBuffer buffer;					//Vulkan buffer object
            VkDeviceMemory memory;				//Vulkan buffer memory
            RaArray<Cell_t> cells;	//Cells in the buffer
        };
        struct MemBufferType {
            CellClass cellClass;				//Class of the cells
            lux::AllocType allocType;	        //Buffer allocation type
            RaArray<MemBuffer> buffers;//Buffers containing the cells
        };

        inline void Cell_t::unmap() { vkUnmapMemory(core::dvc::compute.LD, buffer->memory); }
    }
}