#include "Lynx/Threads/ThreadPool.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Init.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
#include "Lynx/Lynx_config.hpp"
#include <cstdlib>








namespace lnx::thr {
	_lnx_init_var_value_def((RtArray<Thread, uint32>), threads, lnx::thr){
		pVar.resize(LNX_CNF_GLOBAL_THREAD_POOL_SIZE);
	}
	_lnx_init_var_value_def((std::deque<ram::ptr<_pvt::Func_b>>), queue,  lnx::thr){}
	_lnx_init_var_value_def((std::mutex),                         queue_m, lnx::thr){}


	_lnx_init_fun_def(LNX_H_THREAD_POOL, lnx::thr) {
		for(uint32 i = 0; i < LNX_CNF_GLOBAL_THREAD_POOL_SIZE; ++i) g_threads()[i](thrLoop, fwd{ i });
	}








	void thrLoop(uint32 vThrIndex) {
		#ifdef LNX_DBG			//Set the thread name if in debug mode.
			char thrName[64];
			//!^ Only 16 bytes are usable. The size is 64 to not overflow sprintf
			sprintf(thrName, "Lynx | GTP %d", vThrIndex);
			thr::self::setName(thrName);
		#endif

		while(true) {
			g_queue_m().lock();
			if(!g_queue().empty()) {
				ram::ptr<_pvt::Func_b> func = (ram::ptr<_pvt::Func_b>&&)(g_queue().front());	//Save the function data
				g_queue().pop_front();		//Remove the function from the queue
				g_queue_m().unlock();		//Unlock to allow other threads to execute the remaining functions

				func->exec();			//Execute the function
				func->_fence->set();	//After returning, set the fence to notify the eventual waiting thread
				func.free();			//Free the function data
			}
			else g_queue_m().unlock();	//Unlock to allow other threads to execute the remaining functions
			usleep(0);				//Sleep in order to not consume too much CPU while waiting for other functions
			// thr::self::yield();
		}
	}
}