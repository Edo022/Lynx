#include "LuxEngine/Core/Memory/VRam/VRam.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
//TODO use constant for offset








namespace lux::vram{
	luxAutoInit(LUX_H_VMEMORY) {
		core::render::wnd::initWindow( );
		Normal	printf("Creating Instance...                     ");
		core::render::wnd::createInstance( );
		SuccessNoNl printf("ok");

		dbg::checkVk(glfwCreateWindowSurface(core::instance, core::render::wnd::window, nullptr, &core::surface), "Failed to create window surface");
		core::dvc::getPhysical( );
		using namespace __pvt;

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(core::dvc::compute.PD.device, &memoryProperties); //FIXME DIFFERENT QUERY FOR INTEGRATED GPUs
		uint32 systemMemory = memoryProperties.memoryHeaps->size * memoryProperties.memoryHeapCount;

		//Initialize buffer types. Allocate enough cells and buffers to use the whole RAM
		for(uint32 k = 0; k < 2; ++k) { 		//location
			for(uint32 j = 0; j < 2; ++j) { 	//buffer type
				for(uint32 i = 0; i < (uint32)VCellClassIndex::NUM; ++i) { //TODO DIVIDE GRAPHICS
					uint32 buffsNum = systemMemory / buffSize;						//Get max number of cells that can fit in the system memory
					uint32 typeIndex = (i << 2) | (j << 1) | k;
					uint32 cellsPerBuff = buffSize / (uint32)classEnumFromIndex(i);	//Get number of cells in each buffer
					new(&types[typeIndex]) Type_t2{
						.cellClass = classEnumFromIndex(i),									//Set class index
						.memory =  (Cell_t2_csc* )calloc(sizeof(Cell_t2_csc),  buffsNum),				//Allocate the max number of buffers. Initialize them with nullptr
						.cellsPerBuff = cellsPerBuff
					};
					types[typeIndex].cells.init(cellsPerBuff * buffsNum);
				}
			}
		}
		cells.init(systemMemory / (uint64)VCellClass::CLASS_A);
	}
}
