
#pragma once


#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/System/System.h"
#include <thread>



namespace lux::Thread {
	typedef void (*ExecQueueFunc)();
	extern Array<std::thread> threads;
	extern Queue<void*> functions;

	static void __lp_init_thread() {
		threads.resize(G_THREAD_POOL_SIZE);
	}


	static void sendToExecQueue(ExecQueueFunc h) {
		h();
	}
	//struct LuxThreadPool {
	//	uint32 size = 0;				//The number of threads in the thread pool
	//	Queue<std::thread, uint32> threads;	//The threads of the thread pool

	//	LuxThreadPool(uint32 vThreadPoolSize) {
	//		size = vThreadPoolSize;
	//		//threads.resize(size = vThreadPoolSize);
	//	}
	//};

	//TODO


}