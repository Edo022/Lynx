
#pragma once


#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/System/System.h"
#include <thread>


namespace lux::Thread {
	typedef void (*ExecQueueFunc)();
	extern Array<std::thread> threads;
	extern Queue<void*> functions;


	enum Priority : uint16 {
		LUX_PRIORITY_MAX,
		LUX_PRIORITY_HIGH,
		LUX_PRIORITY_LOW,
		LUX_PRIORITY_MIN
	};


	static void __lp_init_thread() {
		threads.resize(G_THREAD_POOL_SIZE);
	}


	//Sends a function to an exec queue of the global thread pool
	//When the function will be executed and which queue it will be assigned to depends on the priority
	////You can add a suspend point to a low priority function with lux::Threads::SuspendPoint(); //TODO 
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
		vFunc(vParams...);
	}
}