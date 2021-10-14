#pragma once
//TODO move to .engine

/**
 * This file contains all the configuration macros of the Lynx Engine
 * Do not modify them if you don't know what you are doing
 * Changes can cause your program to break or behave uncorrectly
 */








// Debug ---------------------------------------------------------------------------------------------------------------------------------//








	//The maximum number of lines the debug backtrace can contain
	//Default: 2048
	#define LNX_CNF_DBG_MAX_BACKTRACE_DEPTH 2048








// Resource pools ------------------------------------------------------------------------------------------------------------------------//








	//Whether or not to use a global thread pool
	//A thread pool can significantly speed up your application and make it easier to divide asynchronous operations among multiple threads
	//It will slightly increase the CPU usage due to thread synchronization
	#define LNX_CNF_GLOBAL_THREAD_POOL //FIXME actually use the option

	//The number of threads in the global thread pool
	//This depends on the number of physical threads of the CPU and the complexity of the application
	//Too many threads will slow down the engine, but too few won't be able to handle all the tasks
	//By default, it contains the same number of thread as the logical threads of the user's CPU
	#define LNX_CNF_GLOBAL_THREAD_POOL_SIZE (lnx::sys::g_cpu().threads)




	//Whether or not to use a global memory pool for RAM allocations
	//A memory pool allocates huge chunks of memory and assigns fixed size sections of it when the application asks for an heap allocation
	//This can significantly improve the performance of allocations, reallocations and deallocations, but will largely increase the memory usage
	//A memory pool is required for memory profiling
	#define LNX_CNF_USE_GLOBAL_MEMORY_POOL //FIXME actually use the option
