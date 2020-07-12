
#pragma once


#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/System/System.h"
#include <thread>


//TODO dont pool with while in fence.wait
//TODO use suspend and resume instead
//TODO use tkill on linux

namespace lux::thr {
	//std::thread::native_handle_type ht = std::thread::native_handle();
	//TODO create LuxThread with platform specific suspend and resume functions
	typedef void (*ExecQueueFunc)();
	extern Array<std::thread> threads;
	extern Queue<void*> maxpq;
	extern Queue<void*> highpq;
	extern Queue<void*> lowpq;
	extern Queue<void*> minpq;


	enum Priority : uint16 {
		LUX_PRIORITY_MAX,
		LUX_PRIORITY_HIGH,
		LUX_PRIORITY_LOW,
		LUX_PRIORITY_MIN
	};
	

	static void __lp_init_thread() {
		threads.resize(G_THREAD_POOL_SIZE);
	}

	//namespace {
		struct qElmBase {
			virtual void exec() = 0;
			Priority priority;
		};
		template<class FuncType, class... ParamTypes> struct qElm : public qElmBase {
			FuncType func; /*uint32 funcTypeSize = sizeof(FuncType);*/
			ParamTypes params;
		};
	//}

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
	//*   
	template<class FuncType, class... ParamTypes> static void sendToExecQueue(const FuncType& vFunc, const Priority vPriority, const ParamTypes& ... vParams) {
		qElm elm;
		elm.func = vFunc;
		elm.priority = vPriority;
		elm.params = vParams;
		//vFunc(vParams...);
	}
}