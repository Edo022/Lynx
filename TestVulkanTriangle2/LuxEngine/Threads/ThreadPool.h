
#pragma once


#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include <thread>



namespace lux {
	struct LuxThreadPool {
		uint32 size = 0;				//The number of threads in the thread pool
		Queue<std::thread, uint32> threads;	//The threads of the thread pool

		LuxThreadPool(uint32 vThreadPoolSize) {
			size = vThreadPoolSize;
			//threads.resize(size = vThreadPoolSize);
		}
	};




	extern lux::Map<LuxThreadPool, uint32> threadPools;
}