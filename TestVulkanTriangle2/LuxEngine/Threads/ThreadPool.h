
#pragma once


#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include <thread>



struct LuxThreadPool{
	uint32 size = 0;				//The number of threads in the thread pool
	LuxQueue<std::thread, uint32> threads;	//The threads of the thread pool

	LuxThreadPool(uint32 vThreadPoolSize) {
		size = vThreadPoolSize;
		//threads.resize(size = vThreadPoolSize);
	}
};




extern LuxMap<LuxThreadPool, uint32> threadPools;