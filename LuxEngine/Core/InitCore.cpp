#include "LuxEngine/Core/LuxAutoInit.hpp"




//Initialize RAM memory pool
#include "LuxEngine/Memory/Ram/Memory.h"
namespace lux::ram{
	MemBufferType* buffers;
	uint32 allocated;


	AutoInit(LUX_H_MEMORY){
		buffers = (MemBufferType*)malloc(sizeof(MemBufferType) * (uint32)CellClassIndex::NUM * (uint32)AllocType::NUM);
		//Init buffer types
		for(uint32 i = 0; i < (uint32)CellClassIndex::NUM; ++i){
			buffers[i].cellClass = (CellClass)classEnumFromIndex((CellClassIndex)i);
			//TODO choose number of buffers based on the system memory
			buffers[i].buffers = Map_NMP_S<MemBuffer, uint32>(32, 8192); //64 buffers per chunk, max 8192 buffers
		}
	}
}




//Initialize GPU memory pool
#include "LuxEngine/Memory/Gpu/VMemory.h"
namespace lux::rem{
	uint32 maxAlloc;
	MemBufferType* buffers;


	AutoInit(LUX_H_VMEMORY){lux::rem::init();}
}




//Initialize system
#include "LuxEngine/System/System.h"
namespace lux::sys{
	String dir::thisDir;	//Path to the current directory //Initialized in init function

	//TODO move to lux::thr
	// uint32		threadNum = lux::sys::threadNum;						//Number of threads in the main CPU
	uint32		threadNum;						//Number of threads in the main CPU


	AutoInit(LUX_H_SYSTEM){
		lux::sys::init();
	}
}




//Initialize thread pool
#include "LuxEngine/Threads/ThreadPool.h"
namespace lux::thr{
	FenceDE					stgAddFence;		//This fence controls the add and read/remove operations of the staging queue
	#ifdef _WIN64
	HANDLE NoInitVar(mngThr);	//The handle of the thread that controls the pool
	#elif defined __linux__
	pthread_t mngThr;
	#endif
	DynArray<ThrPoolElm>		threads;			//The threads of the thread pool with their states and functions
	Map<ThrState, uint32>	thrStates;		//This map contains the states of the threads. It's also used as a linked list to automatically find the next free thread. Max 2048 threads supported


	Queue<ExecFuncDataBase*> maxpq;			//List of maximum priority functions waiting to be executed
	Queue<ExecFuncDataBase*> highpq;			//List of high priority functions waiting to be executed
	Queue<ExecFuncDataBase*> lowpq;			//List of low priority functions waiting to be executed
	Queue<ExecFuncDataBase*> minpq;			//List of minimum priority functions waiting to be executed
	Queue<ExecFuncDataBase*> stg;			//Staging queue


	AutoInit(LUX_H_THREAD_POOL){
		//TODO remove useless debug junk
		int h = LUX_CNF_GLOBAL_THREAD_POOL_SIZE;
		threads = DynArray<ThrPoolElm>(LUX_CNF_GLOBAL_THREAD_POOL_SIZE);
		thrStates.clear();

		maxpq.clear();
		highpq.clear();
		lowpq.clear();
		minpq.clear();
		stg.clear();

		lux::thr::init();
	}
}


#define LUX_H_INIT_CORE
#include <stdlib.h>
// #include "LuxEngine/Core/Core.h"
AutoInit(LUX_H_INIT_CORE){
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wwrite-strings"
	//FIXME use the "libVkLayer_khronos_validation.so" library in the deps folder. Not the one in the default lib location
	//FIXME "LD_LIBRARY_PATH" env variable doesn't work
	putenv(  "VK_LAYER_PATH=./deps/Vulkan_1.2.154.0_linux/x86_64/etc/vulkan/explicit_layer.d");
	putenv("LD_LIBRARY_PATH=./deps/Vulkan_1.2.154.0_linux/x86_64/lib");
	#pragma GCC diagnostic pop
}



// #include "LuxEngine/Core/Core.h"
// namespace lux::core{
// 	struct PreInitializer{ PreInitializer( ){ lux::core::preInit( ); } };
// 	struct PostInitializer{ PostInitializer( ){ lux::core::init(false); } };
// 	//TODO this probably depends on the link order
// 	extern PreInitializer luxPreInitializer; //This variable is used to inizialize the engine before any other variable or function call
// }