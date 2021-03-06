#pragma once








// Common --------------------------------------------------------------------------------------------------------------------------------//








	//LUX_DEBUG
	//This macro defines if the program is compiled in debug or in release mode
	//Lines in _dbg() macro will only be executed in debug mode




	//Whether or not to use precomputed values for goniometric functions
	//fast goniometric functions can be used as a faster but less precise alternative to the standard functions
	//If not defined, fast goniometric functions will be unavailable (fsin, fcos, ffsin...)
	//Those functions will slightly increase the memory usage
#	define LUX_CNF_USE_FFUNCTIONS
	//The precision of the fast goniometric functions: 1/LUX_CNF_FFUNCTIONS_PRECISION
	//More precise values require more memory
	//Default: 100 000 (~7.2MB)
	constexpr int LUX_CNF_FFUNCTIONS_PRECISION = 100000;




	//Whether or not to use a global thread pool
	//A thread pool can significantly speed up a program and make it easier to divide asynchronous operations among multiple threads
	//It will slightly increase the CPU usage during the execution of the program
	//TODO add option
#	define LUX_CNF_USE_GLOBAL_THREAD_POOL
	//The number of logical threads in the global thread pool
	//This depends on the number of physical threads of the CPU and the complexity of the program
	//Too much threads slow down the engine, but too few cannot handle much functions
	//The actual running threads will never be more than the physical threads, but since a thread can be suspended, the pool must be large enough to contain all the active and suspended threads
	//Default: twice the number of physical threads
// #	define LUX_CNF_GLOBAL_THREAD_POOL_SIZE (lux::sys::threadNum * 2)
#	define LUX_CNF_GLOBAL_THREAD_POOL_SIZE (lux::sys::threadNum)




	//Whether or not to use a global memory pool (for memory that is not shared with the GPU. Shared memory has a different pool)
	//A memory pool allocates large chunks of memory to speed up heap memory allocations
	//It could highly increase the memory usage
	//A global memory pool is required for memory profiling
	//TODO add option
#	define LUX_CNF_USE_GLOBAL_MEMORY_POOL
	//The count of the memory chunks allocated by the global memory pool
	//Larger chunks have better performance but uses more memory
	//Every allocation larger than the largest cell count willl be allocated as a single chunk
	//Default: 100 000 000 (100MB)
	//TODO add memory pool
	constexpr int LUX_CNF_GLOBAL_MEMORY_POOL_CHUNK_SIZE = 100000000;


//TODO add memory profiling
//TODO ?
//#	define LUX_CNF_USE_MEMORY_PROFILING





// Advanced. Don't change those if you don't know what you're doing ----------------------------------------------------------------------//








	//The count of the GPU memory pool buffers
	//#LLID ECF0000 buffer limits infos
	//Default: 50 000 000 (50MB)
	//	Theorical max: 4 294 967 296 (~4GB). Sometimes Vulkan does not allow buffers larger than 2GB
	//	Guaranteed max: 2 147 483 648 (~2GB)
	//Larger buffers have better performance, but they increase the memory usage
	//Buffers smaller than (<memBytes> / 1024) are not supported by some devices, therefore the engine will be unable to use part of the memory with that buffer count
	//                        ^
	//                        this is the sum of the shared RAM and the VRAM of the GPU, in bytes
	constexpr int LUX_CNF_GPU_STATIC_BUFFER_SIZE = 50000000;


	////The maximum supported GPU memory (VRAM + shared RAM)
	////
//#	define MAX_SUPPORTED_GPU_MEMORY LUX_CNF_GPU_STATIC_BUFFER_SIZE * 1024