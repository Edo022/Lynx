#include "LuxEngine/Threads/ThreadPool.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include <cstdlib>








namespace lux::thr {
	RtArray<Thread> threads(LUX_CNF_GLOBAL_THREAD_POOL_SIZE);
	std::deque<ram::ptr<__pvt::Func_b>> queue;
	std::mutex queue_m;








	luxAutoInit(LUX_H_THREAD_POOL) {
		for(uint32 i = 0; i < LUX_CNF_GLOBAL_THREAD_POOL_SIZE; ++i) threads[i](thrLoop, L{ i });
	}








	void thrLoop(uint32 vThrIndex) {
		#ifdef LUX_DEBUG			//Set the thread name if in debug mode.
			char thrName[16];
			sprintf(thrName, "Lux | GTP %d", vThrIndex);
			thr::self::setName(thrName);
		#endif

		while(true) {
			queue_m.lock();
			if(!queue.empty()) {
				ram::ptr<__pvt::Func_b> func = (ram::ptr<__pvt::Func_b>&&)(queue.front());	//Save the function data
				queue.pop_front();		//Remove the function from the queue
				queue_m.unlock();		//Unlock to allow other threads to execute the remaining functions

				func->exec();			//Execute the function
				func->_fence->set();	//After returning, set the fence to notify the eventual waiting thread
				func.free();			//Free the function data
			}
			else queue_m.unlock();	//Unlock to allow other threads to execute the remaining functions
			usleep(0);				//Sleep in order to not consume too much CPU while waiting for other functions
			// thr::self::yield();
		}
	}
}