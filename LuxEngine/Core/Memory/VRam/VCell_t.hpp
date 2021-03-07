#pragma once
#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Memory/VRam/VClasses.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"







namespace lux{
    namespace vram{
        struct Cell_t2_csc{
            VkBuffer buffer;		    //Vulkan buffer object                          //8
            VkDeviceMemory memory;	    //Vulkan buffer memory                          //8
        };
        struct Cell_t2{
            uint16 typeIndex;	        //Buffer allocation type                        //2
			uint32 cellIndex;		    //Index of the cell in the cells array          //4
			uint32 localIndex;		    //Index of the cell in the type allocations     //4
            uint32 localOffset;         //Offset of the memory in the buffer in byes    //4
			uint32 cellSize;		    //Size of the cell in bytes                     //4
            Cell_t2_csc csc;            //Vulkan allocation data                        //16
        };


        struct Type_t2{
            VCellClass cellClass;	    //Class of the cells
            Cell_t2_csc* memory;        //Vulkan allocation data (Copied by each cell)

			RaArrayC<bool> cells;	    //TODO use optimized uint64 array
			uint32 cellsPerBuff;	    //Number of cells in each buffer
            std::mutex m;               //Mutex for multithread access
        };

		extern Type_t2 types[];		    //Buffer types
        extern RaArrayC<Cell_t2> cells; //Preallocated cells
        extern std::mutex cells_m;      //Mutex for multithread access
    }
}