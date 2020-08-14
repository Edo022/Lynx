
#pragma once

#include "LuxEngine/LuxEngine_config.h"
#include "LuxEngine/System/System.h"

#include "LuxEngine/Types/LuxFenceDE.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/Containers/LuxQueue.h"
#include <tuple>
#include <thread>





//TODO create a fence that uses suspend and resume instead of while checks
//TODO use tkill on linux
//TODO create LuxThread with platform specific suspend and resume functions

namespace lux::thr {
	//TODO fix queue priority
	enum Priority : uint16 {
		LUX_PRIORITY_MAX = 4,	//execute as soon as possible, eventually suspending the execution of lower priority functions
		LUX_PRIORITY_HIGH = 3,	//execute only after all the max priority functions have been executed
		LUX_PRIORITY_LOW = 2,	//execute only after all the higher priority functions have been executed
		LUX_PRIORITY_MIN = 1	//execute when there are no higher priority functions left
	};
	enum class ThrState : uint8 {
		RUNNING,
		FREE,
		SUSPENDED
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
			*fence = true;
			//printf("%d", std::apply(func, params));
		}
		FType func;
		Priority priority;
		bool* fence;
		std::tuple<PTypes...> params;
	};


	struct ThrPoolElm{
		std::thread* thr{ nullptr };			//The actual thread
		ExecFuncDataBase* exec{ nullptr };		//A pointer to the data of the function to execute
	};




	extern FenceDE stgAddFence;
	extern HANDLE mngThr;
	extern Array<ThrPoolElm> threads;
	extern Map<ThrState, uint32> thrStates;
	extern Queue<ExecFuncDataBase*> maxpq;
	extern Queue<ExecFuncDataBase*> highpq;
	extern Queue<ExecFuncDataBase*> lowpq;
	extern Queue<ExecFuncDataBase*> minpq;
	extern Queue<ExecFuncDataBase*> stg;




	void __lp_thr_loop(const uint32 vThrIndex);
	void __lp_thr_mng( );
	void __lp_init_thread( );

	//Sends a function to an exec queue of the global thread pool
	//When the function will be executed and which queue it will be assigned to depends on its priority
	//A low priority function can be suspended to free a thread and execute one with higher priority
	//*   vFunc     | the function to execute
	//*   vPriority | the priority of the function (LUX_PRIORITY_MAX, LUX_PRIORITY_HIGH...)
	//*   pReturn   | a pointer to the variable where to store the function return value. Use nullptr for void functions
	//*   pFence    | a pointer to a bool variable that will be set to true when the thread returns. Use nullptr if you dont need one
	//*   vParams   | the parameters of the function call. Their types must be the same as the function declaration
	//The maximum number of functions executed at the "same time" is defined by LUX_CNF_GLOBAL_THREAD_POOL_SIZE (see LuxEngine_config.h)
	//The actual number of running threads is limited to the number of physical threads in the CPU
	//Use pointers or references to improve performance. The parameters have to be copied several times during the process
	template<class FType, class... PTypes> void sendToExecQueue(FType vFunc, const Priority vPriority, bool* const pFence, PTypes ...vParams) {
		//TODO add return ptr
		ExecFuncData<FType, PTypes...>* cntv = new ExecFuncData<FType, PTypes...>;	//Create the function data structure
		cntv->func = vFunc;							//Set the function
		cntv->fence = pFence;						//Set the fence
		cntv->params = std::make_tuple(vParams...);	//Set the parameters

		stgAddFence.startSecond( );
		stg.pushFront(cntv);						//Assign the data to the staging queue
		stgAddFence.endSecond( );
		ResumeThread(mngThr);						//#LLID THR0001 Resume the thread (it suspended itself after assigning the last functions)
	}
}