
#include "LuxEngine/Threads/ThreadPool.h"
#include "LuxEngine/Core/Core.h"

//TODO "so that whatever file is being initialized, the execution get caught by the post initializer and it inizializes everything the engine needs"

#pragma optimize("", off)
PostInitializer(LUX_H_THREAD_POOL);
#pragma optimize("", on)
namespace lux::thr {
	FenceDE					NoInitLux(stgAddFence);		//This fence controls the add and read/remove operations of the staging queue
	HANDLE					NoInitVar(mngThr);			//The handle of the thread that controls the pool
	Array<ThrPoolElm>		NoInitLux(threads);			//The threads of the thread pool with their states and functions
	Map<ThrState, uint32>	NoInitLux(thrStates);		//This map contains the states of the threads. It's also used as a linked list to automatically find the next free thread. Max 2048 threads supported


	Queue<ExecFuncDataBase*> NoInitLux(maxpq);			//List of maximum priority functions waiting to be executed
	Queue<ExecFuncDataBase*> NoInitLux(highpq);			//List of high priority functions waiting to be executed
	Queue<ExecFuncDataBase*> NoInitLux(lowpq);			//List of low priority functions waiting to be executed
	Queue<ExecFuncDataBase*> NoInitLux(minpq);			//List of minimum priority functions waiting to be executed
	Queue<ExecFuncDataBase*> NoInitLux(stg);			//Staging queue




	void preInit( ){
		threads.Array::Array(LUX_CNF_GLOBAL_THREAD_POOL_SIZE);
		thrStates.Map::Map(2048, 2048);

		maxpq.Queue::Queue( );
		highpq.Queue::Queue( );
		lowpq.Queue::Queue( );
		minpq.Queue::Queue( );
		stg.Queue::Queue( );
	}



	void __lp_thr_loop(const uint32 vThrIndex) {
		{ //Set thread name for debugging
			#pragma warning( disable:4996 )
			luxDebug(String ThrNum = "\0 2 4 6 8 0");
			luxDebug(String thrNumStrL = _itoa(vThrIndex, ThrNum.begin( ), 10));
			luxDebug(String thrName = "\tLuxEngine  |  GTP ");
			luxDebug(thrName += thrNumStrL);
			luxDebug(wchar_t lthn[100]);
			luxDebug(mbstowcs(lthn, thrName.begin( ), thrName.size( ) + 1));
			luxDebug(SetThreadDescription(GetCurrentThread( ), lthn));
			#pragma warning( default:4996 )
		}

		SuspendThread(GetCurrentThread( ));
		while(true) {
			0;													//#LLID THR0000 The thread will continue from here when it's resumed
			if(thrStates[vThrIndex] == ThrState::RUNNING){		//If a function was assigned to the thread
				//TODO save return
				threads[vThrIndex].exec->exec( );					//Execute it and save the retun value in the return address
				delete(threads[vThrIndex].exec);					//Free the pointer to the function data
				thrStates.remove(vThrIndex);						//Remove the thread state from the map
			}

			SuspendThread(GetCurrentThread( ));					//Suspend the thread
		}
	}





	void __lp_thr_mng( ) {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  GTP MNG"));
		SuspendThread(GetCurrentThread( ));
		while(true){
			0;															//#LLID THR0001 The thread will continue from here when it's resumed
			if(!stg.empty( )){											//Check if new functions were added to the queues
				stgAddFence.startFirst( );
				while(stg.size( ) > 0){										//For each element of the queue
					//TODO do something if there are no enought threads
					if(thrStates.usedSize( ) < sys::threadNum){					//If there is a free thread
						uint32 thrIndex = thrStates.add(ThrState::RUNNING);			//Set its state to RUNNING and save its index (automatically calculated by the add() function)
						threads[thrIndex].exec = stg.front( );						//Set its exec data
						stg.popFront( );											//Remove the exec data from the queue
						ResumeThread(threads[thrIndex].thr->native_handle( ));		//#LLID THR0000 Resume the thread (it suspended itself after executing the last function)
					}
				}
				stgAddFence.endFirst( );
				SuspendThread(GetCurrentThread( ));							//Suspend the thread (the mng thread, not the one that was free)
			}
		}
	}




	void init( ) {
		for(uint32 i = 0; i < threads.size( ); ++i){					//For each thread
			threads[i].thr = new std::thread(__lp_thr_loop, i);			//Initialize it with the thread loop function
			thrStates.add(ThrState::FREE);								//Add an element to the states Map. A Map is used to improve the performance by avoiding to search for a free thread
			//Suspended in function
		}
		for(uint32 i = 0; i < threads.size( ); ++i){ thrStates.remove(i); }		//Remove all the elements of the states map (the map will remain the same size but it will have n free items)
		std::thread mngThrv(__lp_thr_mng); 										//Start mng thread and duplicate the handle (a thread handle becomes invalid when detached)
		DuplicateHandle(GetCurrentProcess( ), mngThrv.native_handle( ), GetCurrentProcess( ), &mngThr, DUPLICATE_SAME_ACCESS, 0, 0);
		mngThrv.detach( );
	}
}