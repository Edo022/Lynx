
#pragma once


#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/System/System.h"
#include <tuple>
#include <thread>


//TODO dont pool with while in fence.wait
//TODO use suspend and resume instead
//TODO use tkill on linux

namespace lux::thr {
	//TODO create LuxThread with platform specific suspend and resume functions
	enum Priority : uint16 {
		LUX_PRIORITY_MAX,	//execute as soon as possible, eventually suspending the execution of lower priority functions
		LUX_PRIORITY_HIGH,	//execute only after all the max priority functions have been executed
		LUX_PRIORITY_LOW,	//execute only after all the higher priority functions have been executed
		LUX_PRIORITY_MIN	//execute when there are no higher priority functions left
	};
	enum class ThrState : uint8 {
		RUNNING =   1,
		FREE =	    2,
		SUSPENDED = 3
	};



	//Base function of ExecFuncData to allow differently templated ExecFuncData structs to be saved in the same array
	struct ExecFuncDataBase {
		virtual void exec( ) {};
	};
	//Executable Function Data
	//This struct stores a function call with its parameters
	//The exec() function executes the function with the saved parameters 
	//The return value is copied in the return pointer
	template<class FType, class ...PTypes> struct ExecFuncData : public ExecFuncDataBase {
		void exec( ) final override {
			std::apply(func, params);
			//printf("%d", std::apply(func, params));
		}
		FType func;
		std::tuple<PTypes...> params;
	};



	struct ThrPoolElm{
		std::thread* thr{ nullptr };			//The actual thread
		ThrState thrState{ ThrState::FREE };	//The state of the thread //TODO remove
		ExecFuncDataBase* exec;					//A pointer to the data of the function to execute
	};


	extern bool update;							//True when new functions are added to the queues
	extern HANDLE mngThr;
	extern Array<ThrPoolElm> threads;			//The threads of the thread pool with their states and functions
	extern Map<ThrState, uint32> thrStates;		//Idk it's difficult to explain. look at the code //TODO 
	extern Queue<ExecFuncDataBase*> maxpq;		//List of maximum priority functions waiting to be executed
	extern Queue<ExecFuncDataBase*> highpq;		//List of high priority functions waiting to be executed
	extern Queue<ExecFuncDataBase*> lowpq;		//List of low priority functions waiting to be executed
	extern Queue<ExecFuncDataBase*> minpq;		//List of minimum priority functions waiting to be executed




	static void __lp_thr_loop(const uint32 vThrIndex) {
		{ //Set thread name for debugging
			String ThrNum = "\0 2 4 6 8 0";
			luxDebug(String thrNumStrL = itoa(vThrIndex, ThrNum.begin( ), 10));
			luxDebug(String thrName = "\tLuxEngine  |  GTP ");
			luxDebug(thrName += thrNumStrL);
			luxDebug(wchar_t lthn[100]);
			luxDebug(mbstowcs(lthn, thrName.begin( ), thrName.size( ) + 1));
			luxDebug(SetThreadDescription(GetCurrentThread( ), lthn));
		}

		SuspendThread(GetCurrentThread());
		while(true) {
			0;													//#LLID THR0000 The thread will continue from here when it's resumed
			if(thrStates[vThrIndex] == ThrState::RUNNING){		//If a function was assigned to the thread
				//TODO save return							
				threads[vThrIndex].exec->exec( );					//Execute it and save the retun value in the return address
				//TODO use free
				delete(threads[vThrIndex].exec);					//Free the pointer to the function data
			}
			SuspendThread(GetCurrentThread( ));					//Suspend the thread
		}
	}



	static void __lp_thr_mng() {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  GTP MNG"));
		SuspendThread(GetCurrentThread());
		while(true){
			sleep(5);													//#LLID THR0001 The thread will continue from here when it's resumed
			if(update){													//Check if new functions were added to the queues
				while(maxpq.size( ) > 0){									//For each element of the queue
					//TODO
					if(thrStates.usedSize( ) < sys::threadNum){					//If there is a free thread
						uint32 thrIndex = thrStates.add(ThrState::RUNNING);			//Set its state to RUNNING and save its index (automatically calculated by the add() function)
						threads[thrIndex].exec = maxpq.front();						//Set its exec data
						maxpq.popFront( );											//Remove the exec data from the queue
						ResumeThread(threads[thrIndex].thr->native_handle());		//#LLID THR0000 Resume the thread (it suspended itself after executing the last function)
					}
				}
				//TODO dont set if there are no enought threads
				update = false; 											//No need to update again		//"!update" would be way faster to write...
				SuspendThread(GetCurrentThread( ));							//Suspend the thread (the mng thread, not the one that was free)
			}
		}
	}




	static void __lp_init_thread( ) {
		threads.resize(G_THREAD_POOL_SIZE);															//Resize the thread pool
		for(int32 i = 0; i < threads.size( ); ++i){													//For each thread
			//TODO use malloc
			threads[i].thr = new std::thread(__lp_thr_loop, i);											//Initialize it with the thread loop function
			threads[i].thrState = (i < lux::sys::threadNum) ? ThrState::FREE : ThrState::SUSPENDED;		//Set its state
			thrStates.add(ThrState::FREE);																//Add an element to the states map
			//Suspended in function
		}
		for(int32 i = 0; i < threads.size( ); ++i){ thrStates.remove(i); }							//Remove all the elements of the states map (the map will remain the same size but it will have n free items)
		//ResumeThread(threads[0].thr->native_handle());
		std::thread mngThrv(__lp_thr_mng); 															//Start mng thread
		DuplicateHandle(GetCurrentProcess( ), mngThrv.native_handle( ), GetCurrentProcess( ), &mngThr, DUPLICATE_SAME_ACCESS, 0, 0);		//Duplicate handle (thread hancdle becomes invalid when detached)
		mngThrv.detach( );																			//Detach thread




		//std::thread mngThrv(__lp_thr_mng);
		//DuplicateHandle(GetCurrentProcess( ), mngThrv.native_handle( ), GetCurrentProcess( ), &mngThr, DUPLICATE_SAME_ACCESS, 0, 0);
		//mngThrv.detach( );
	}






	//Sends a function to an exec queue of the global thread pool
	//When the function will be executed and which queue it will be assigned to depends on its priority
	//A low priority function can be suspended to free a thread and execute a higher priority one
	//*   vFunc     | the function to execute
	//*   vPriority | the priority of the function (LUX_PRIORITY_MAX, LUX_PRIORITY_HIGH...)
	//*   pReturn   | a pointer to the variable where to store the function return value. Use nullptr for void functions
	//*   vParams   | the parameters of the function call. Their types must be the same as the function declaration
	//The maximum number of functions executed at the "same time" is defined by G_THREAD_POOL_SIZE (see LuxEngine_config.h)
	//The actual number of running threads is limited to the number of physical threads in the CPU
	template<class FType, class... PTypes> static void sendToExecQueue(FType vFunc, const Priority vPriority, PTypes ...vParams) {
		//TODO add return ptr
		ExecFuncData<FType, PTypes...>* cntv = new ExecFuncData<FType, PTypes...>;	//Create the function data structure
		cntv->func = vFunc;															//Set the function
		cntv->params = std::make_tuple(vParams...);									//Set the parameters
		
		//TODO add push fences
		switch(vPriority){															//Assign the data to a queue based on its priority
			case LUX_PRIORITY_MAX:	maxpq.pushFront(cntv);	break;
			case LUX_PRIORITY_HIGH:	highpq.pushFront(cntv);	break;
			case LUX_PRIORITY_LOW:	lowpq.pushFront(cntv);	break;
			case LUX_PRIORITY_MIN:	minpq.pushFront(cntv);	break;
		}
		update = true;																//Set to true the update variable so the mng thread can assign the data to a thread
		ResumeThread(mngThr);										//#LLID THR0001 Resume the thread (it suspended itself after assigning the last functions)
	}
}