
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
		LUX_PRIORITY_MAX,
		LUX_PRIORITY_HIGH,
		LUX_PRIORITY_LOW,
		LUX_PRIORITY_MIN
	};



	
	//Base function of ExecFuncData to allow differently templated ExecFuncData structs to be saved in the same array
	struct ExecFuncDataBase {
		virtual void exec() {};
	};

	//Executable Function Data
	//This struct stores a function call with its parameters
	//The exec() function executes the function with the saved parameters 
	//The return value is copied in the return pointer
	template<class FType, class ...PTypes> struct ExecFuncData : public ExecFuncDataBase {
		void exec() final override {
			std::apply(func, params);
			//printf("%d", std::apply(func, params));
		}
		FType func;
		std::tuple<PTypes...> params;
	};




	typedef void (*ExecQueueFunc)();
	extern Array<std::thread*> threads;
	extern Queue<ExecFuncDataBase*> maxpq;
	extern Queue<ExecFuncDataBase*> highpq;
	extern Queue<ExecFuncDataBase*> lowpq;
	extern Queue<ExecFuncDataBase*> minpq;
	



	static void __lp_thr_loop(const uint32 vThrID) {
		luxDebug(char thrNumStr[100]);
		luxDebug(String thrNumStrL = itoa(vThrID, thrNumStr, 10));
		luxDebug(String thrName = "\tLuxEngine  |  GTP ");
		luxDebug(thrName += thrNumStrL);
		luxDebug(wchar_t lthn[100]);
		luxDebug(const char* thrn = thrName.begin( ));
		luxDebug(mbstowcs(lthn, thrn, strlen(thrn) + 1));
		luxDebug(SetThreadDescription(GetCurrentThread( ), lthn));
		static bool _h_ = true;

		while(true) {
			sleep(10);
			if(!maxpq.empty( )) {
				maxpq.front( )->exec( );
				maxpq.popFront( );
			}
		}
	}

	static void __lp_init_thread( ) {
		threads.resize(G_THREAD_POOL_SIZE);
		for(int32 i = 0; i < threads.size( ); ++i){
			threads[i] = new std::thread(__lp_thr_loop, i);
			sleep(100);
		}
	}



	//Sends a function to an exec queue of the global thread pool
	//When the function will be executed and which queue it will be assigned to depends on the priority
	//*   vFunc: the function to execute
	//*   vPriority: the priority of the function
	//*       LUX_PRIORITY_MAX : execute as soon as possible, eventually suspending the execution of lower priority functions
	//*       LUX_PRIORITY_HIGH: execute only after all the max priority functions have been executed
	//*       LUX_PRIORITY_LOW : execute only after all the higher priority functions have been executed
	//*       LUX_PRIORITY_MIN : execute when there are no higher priority functions left 
	//*       the number of functions executed at the same time depends on G_THREAD_POOL_SIZE (see LuxEngine_config.h)
	//*   vParams: the parameters of the function call. Their types must be the same as the function declaration
	template<class FType, class... PTypes> static void sendToExecQueue(FType vFunc, const Priority vPriority, PTypes ...vParams) {
		ExecFuncData<FType, PTypes...>* cntv = new ExecFuncData<FType, PTypes...>;
		cntv->func = vFunc;
		cntv->params = std::make_tuple(vParams...);

		//TODO add push fences
		maxpq.pushFront(cntv);
	}
}