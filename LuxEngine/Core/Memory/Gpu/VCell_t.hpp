#include "LuxEngine/Core/Memory/Shared.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"







namespace lux{
	enum class AllocType : uint32 {
		VRamStorage = 0b00,	//Storage buffer in dedicated GPU memory
		VRamUniform = 0b01,	//Uniform buffer in dedicated GPU memory
		RamStorage  = 0b10,	//Storage buffer in shared RAM memory
		RamUniform  = 0b11,	//Uniform buffer in shared RAM memory
		NUM					//Number of LUX_ALLOC_TYPE values
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










        struct Cell_t2{
            lux::AllocType typeIndex;	        //Buffer allocation type
			uint32 cellIndex;		//Index of the cell in the cells array						//8
			uint32 localIndex;		//Index of the cell in the type allocations					//12
			uint32 cellSize;		//Size of the cell in bytes									//16
        };


        struct Type_t2{
            CellClass cellClass;				//Class of the cells
            lux::AllocType allocType;	        //Buffer allocation type

            VkBuffer buffer;					//Vulkan buffer object
            VkDeviceMemory memory;				//Vulkan buffer memory

			RaArrayC<bool> cells;	//TODO use optimized uint64 array
			uint32 cellsPerBuff;	//Number of cells in each buffer
            std::mutex m; //FIXME REMOVE
        };

        extern RaArrayC<Cell_t2> cells;
    }
}