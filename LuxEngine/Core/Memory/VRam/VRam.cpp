#include "LuxEngine/Core/Memory/VRam/VRam.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/System/SystemInfo.hpp"
//TODO use constant for offset








namespace lux::sys{
	//! This function should be the initialization function for the VRam variables,
	//! but it also has to initialize the VRAM informations in the sys namespace, which happens to be a const.
	//! And it has to do it after getting the device infos, but before allocating the cells.
	//! So the function is a lambda that does both by directly initializing the variables and returning a sys::VRamInfo to the const

	// luxAutoInit(LUX_H_VMEMORY) {
	const VRamInfo vram = [](){
		core::render::wnd::initWindow();
		Normal	printf("Creating Instance...                     ");
		core::render::wnd::createInstance();
		SuccessNoNl printf("ok");

		dbg::checkVk(glfwCreateWindowSurface(core::instance, core::render::wnd::window, nullptr, &core::surface), "Failed to create window surface");
		core::dvc::getPhysical();
		using namespace vram::__pvt;

		//Get VRAM informations
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(core::dvc::compute.PD.device, &memoryProperties); //FIXME DIFFERENT QUERY FOR INTEGRATED GPUs
		VRamInfo _vram;
		_vram.heaps.num =  memoryProperties.memoryHeapCount;
		for(int i = 0; i < memoryProperties.memoryHeapCount; ++i){
			if(memoryProperties.memoryHeaps[i].flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT){
				_vram.size += memoryProperties.memoryHeaps[i].size;
			}
			_vram.heaps[i] = { .size = memoryProperties.memoryHeaps[i].size, .flags = memoryProperties.memoryHeaps[i].flags };
		}


		//Initialize buffer types. Allocate enough cells and buffers to use the whole RAM
		for(uint32 k = 0; k < 2; ++k) { 		//location
			for(uint32 j = 0; j < 2; ++j) { 	//buffer type
				for(uint32 i = 0; i < (uint32)VCellClassIndex::NUM; ++i) {
					uint32 buffsNum = _vram.size / buffSize;						//Get max number of cells that can fit in the system memory
					uint32 typeIndex = (i << 2) | (j << 1) | k;
					uint32 cellsPerBuff = buffSize / (uint32)classEnumFromIndex(i);	//Get number of cells in each buffer
					new(&vram::types[typeIndex]) vram::Type_t2{
						.cellClass = classEnumFromIndex(i),									//Set class index
						.memory =  (vram::Cell_t2_csc* )calloc(sizeof(vram::Cell_t2_csc),  buffsNum),				//Allocate the max number of buffers. Initialize them with nullptr
						.cellsPerBuff = cellsPerBuff
					};
					vram::types[typeIndex].cells.init(cellsPerBuff * buffsNum);
				}
			}
		}
		vram::cells.init(_vram.size / (uint64)vram::VCellClass::CLASS_A);
		return _vram;
	}();
}
