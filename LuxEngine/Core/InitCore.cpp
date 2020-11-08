#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Memory/Gpu/VMemory.h"
#include "LuxEngine/System/System.h"
#include "LuxEngine/Threads/ThreadPool.h"




//Initialize RAM memory pool
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
namespace lux::rem{
	uint32 maxAlloc;
	MemBufferType* buffers;


	AutoInit(LUX_H_VMEMORY){lux::rem::init();}
}




//Initialize system 
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


#include <stdlib.h>
#define HH
AutoInit(HH){
	putenv("VK_LAYER_PATH=deps/Vulkan_1.2.154.0_linux/x86_64/etc/vulkan/explicit_layer.d");
	putenv("LD_LIBRARY_PATH=deps/Vulkan_1.2.154.0_linux/x86_64/lib");
}