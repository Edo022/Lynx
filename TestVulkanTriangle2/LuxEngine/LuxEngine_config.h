#pragma once




// Common --------------------------------------------------------------------------------------------------------------------------------//




	//This macro defines if the program is compiled in debug or in release mode
	//In debug mode, lines in luxDebug() macro will be executed
#	ifndef NDEBUG 
#		define LUX_DEBUG
#	else
#		undef LUX_DEBUG
#	endif


	//The precision of the goniometric functions precomputed values: 1/FUNC_PRECISION
	//0 means the engine will not use precomputed values and fast goniometric functions will be unavailable (fsin, fcos, ffsin...)
	//Default: 100 000 (7.2MB)
#	define FUNC_PRECISION 100000


	//The number of logical threads in the global thread pool
	//This depends on the number of physical threads of the CPU and the complexity of the program
	//Too much threads slow down everything, but too few cannot handle much functions
	//The optimal value is around 2 to 4 times the number of physical threads
#	define G_THREAD_POOL_SIZE lux::sys::threadNum * 4




// Advanced. Don't change those if you don't know what you're doing ----------------------------------------------------------------------//




	//The size of the GPU buffers
	//#LLID ECF0000 buffer limits infos
	//Default: 50 000 000 (50MB)	
	//	Theorical max: 4 294 967 296 (~4GB). Sometimes Vulkan does not allow so large buffers
	//	Guaranteed max: 2 147 483 648 (~2GB)
	//Larger buffers have better performance, but they increase the memory usage
	//Buffers smaller than (<memBytes> / 1024) are not supported by some devices, therefore the engine will be unable to use part of the memory
	//                        ^ 
	//                        this is the sum of the shared RAM and the VRAM of the GPU, in bytes
#	define GPU_STATIC_BUFFER_SIZE 50000000


	////The maximum supported GPU memory (VRAM + shared RAM)
	////
//#	define MAX_SUPPORTED_GPU_MEMORY GPU_STATIC_BUFFER_SIZE * 1024