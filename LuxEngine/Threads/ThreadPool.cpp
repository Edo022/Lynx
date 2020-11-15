#include "LuxEngine/Threads/ThreadPool.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include <stdlib.h>








namespace lux::thr {
	void* __lp_thr_loop(void* vThrIndex) {
		//TODO add + operators with integers and float values for lux strings
		//FIXME add thread number in thread name
		luxDebug(pthread_setname_np(pthread_self(), "Lux | GTP "));

		// __lp_suspend_thr(__lp_get_thr( ));
		pthread_kill(pthread_self(), SIGSTOP);		//Suspend the thread
		// while(true) sleep(10);
		while(true) {
			0;													//#LLID THR0000 The thread will continue from here when it's resumed
			if(thrStates[*((uint32*)vThrIndex)] == ThrState::RUNNING){		//If a function was assigned to the thread
				//TODO save return
				threads[*((uint32*)vThrIndex)].exec->exec( );					//Execute it and save the retun value in the return address
				delete(threads[*((uint32*)vThrIndex)].exec);					//Free the pointer to the function data
				thrStates.remove(*((uint32*)vThrIndex));						//Remove the thread state from the map
			}

			pthread_kill(pthread_self(), SIGSTOP);		//Suspend the thread
		}
		return nullptr;
	}





	void __lp_thr_mng( ) {
		luxDebug(pthread_setname_np(pthread_self(), "Lux | GTP Mng"));

		pthread_kill(pthread_self(), SIGSTOP);

		while(true){
			0;															//#LLID THR0001 The thread will continue from here when it's resumed
			if(!stg.empty( )){											//Check if new functions were added to the queues
				stgAddFence.startFirst( );
				while(stg.size( ) > 0){										//For each element of the queue
					//TODO do something if there are no enought threads
					if(thrStates.usedCount( ) < sys::threadNum){					//If there is a free thread
						uint32 thrIndex = thrStates.add(ThrState::RUNNING);			//Set its state to RUNNING and save its index (automatically calculated by the add() function)
						threads[thrIndex].exec = stg.front( );						//Set its exec data
						stg.popFront( );											//Remove the exec data from the queue
						pthread_kill(threads[thrIndex].thr, SIGCONT);
					}
				}
				stgAddFence.endFirst( );
				pthread_kill(pthread_self(), SIGSTOP);
			}
		}
	}
}