#include "Lynx/Core/Memory/VRam/VRam.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/System/SystemInfo.hpp"
//TODO use constant for offset








namespace lnx::sys{ //FIXME update comment
	//! This function should be the initialization function for the VRam variables,
	//! but it also has to initialize the VRAM informations in the sys namespace, which happens to be a const.
	//! And it has to do it after getting the device infos, but before allocating the cells.
	//! So the function is a lambda that does both by directly initializing the variables and returning a sys::VRamInfo to the const

	__init_var_const_def(const GpuInfo, vram){
		//Get GPU informations
		GpuInfo _vram;
		_vram.name = core::dvc::graphics.pd.properties.deviceName;
		_vram.type = (core::dvc::graphics.pd.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) ? sys::DeviceType::eDiscrete : sys::DeviceType::eIntegrated;
		_vram.maxWgSize[0] = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupSize[0];
		_vram.maxWgSize[1] = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupSize[1];
		_vram.maxWgSize[2] = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupSize[2];
		_vram.maxWgInvoc   = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupInvocations;
		_vram.maxWgs[0]    = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupCount[0];
		_vram.maxWgs[1]    = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupCount[1];
		_vram.maxWgs[2]    = core::dvc::graphics.pd.properties.limits.maxComputeWorkGroupCount[2];

		//Get VRAM informations
		auto memoryProperties = core::dvc::graphics.pd.device.getMemoryProperties(); //FIXME DIFFERENT QUERY FOR INTEGRATED GPUs
		_vram.heaps.num =  memoryProperties.memoryHeapCount;
		for(uint32 i = 0; i < memoryProperties.memoryHeapCount; ++i){
			if(memoryProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal){
				_vram.size += memoryProperties.memoryHeaps[i].size;
			}
			_vram.heaps[i] = { .size = memoryProperties.memoryHeaps[i].size, .flags = memoryProperties.memoryHeaps[i].flags };
		}

		return _vram;
	}



	// LnxAutoInit() {
	// 	using namespace vram::__pvt;

	// 	//Initialize buffer types. Allocate enough cells and buffers to use the whole RAM
	// 	for(uint32 k = 0; k < 2; ++k) { 									//Loop location
	// 		for(uint32 j = 0; j < 2; ++j) { 									//Loop buffer type
	// 			for(uint32 i = 0; i < (uint32)VCellClassIndex::eNum; ++i) {			//Loop cell size
	// 				uint32 buffsNum = sys::vram.size / buffSize;						//Get max number of cells that can fit in the system memory
	// 				uint32 typeIndex = (i << 2) | (j << 1) | k;							//Calculate buffer type index
	// 				uint32 cellsPerBuff = buffSize / (uint32)classEnumFromIndex(i);		//Get number of cells in each buffer
	// 				new(&vram::types[typeIndex]) vram::Type_t2{							//Create new type struct
	// 					.cellClass = classEnumFromIndex(i),									//Set class index
	// 					.memory =  (vram::Cell_t2_csc* )calloc(sizeof(vram::Cell_t2_csc),  buffsNum),//Allocate the max number of buffers. Initialize them with nullptr
	// 					.cellsPerBuff = cellsPerBuff										//Set the maximum number of cells per buffer
	// 				};
	// 				vram::types[typeIndex].cells.init(cellsPerBuff * buffsNum);
	// 			}
	// 		}
	// 	}
	// 	vram::cells.init(sys::vram.size / (uint64)vram::VCellClass::eA);
	// }
}
