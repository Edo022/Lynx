#pragma once
#define LUX_H_THREAD_POOL
#include "LuxEngine/LuxEngine_config.hpp"
#include "LuxEngine/System/System.hpp"

#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Types/Containers/LuxQueue.hpp"
#include <tuple>
#include <thread>




//TODO create a fence that uses suspend and resume instead of while checks
//TODO create LuxThread with platform specific suspend and resume functions


//TODO ci sono diversi tipi di superfici
//TODO questi tipi vengono calcolati automaticamente e salvati nei dati dell'oggetto
//TODO così in runtime non serve calcolarli. devono essere ricalcolati in background se l'oggetto viene modificato
//TODO ogni superficie ha dei punti caratteristici da cui dipendono le sue proprietà fisiche (non grafica)
//TODO i punti vengono calcolati dopo aver trovato il tipo e sono probabilment indici a dei punti normali
//TODO nn
//TODO

//TODO add function to send multiple functions to the thread pool to allow them to be execute simultaneously
//TODO while preventing their threads to go in deadlock if synchronized



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
		virtual void exec() {};
	};
	//Executable Function Data
	//This struct stores a function call with its parameters
	//The exec() function executes the function with the saved parameters
	//The return value is copied in the return pointer
	template<class FType, class ...PTypes> struct ExecFuncData : public ExecFuncDataBase {
		void exec( ) final override {
			//if(_return) *_return = std::apply(func, params);
			/*else*/ std::apply(func, params);
			if(fence) *fence = true;
		}
		FType func;
		Priority priority;
		//FType* _return;
		bool* fence;
		std::tuple<PTypes...> params;
	};


	struct ThrPoolElm{
		// std::thread* thr{ nullptr };			//The actual thread
		pthread_t thr;			//The actual thread
		ExecFuncDataBase* exec{ nullptr };		//A pointer to the function data
	};




	extern FenceDE stgAddFence;
	//TODO move to system header
	extern win10(HANDLE)linux(pthread_t) mngThr;
	extern RtArray<ThrPoolElm> threads;
	extern RaArray<ThrState, uint32> thrStates;
	extern Queue<ExecFuncDataBase*> maxpq;
	extern Queue<ExecFuncDataBase*> highpq;
	extern Queue<ExecFuncDataBase*> lowpq;
	extern Queue<ExecFuncDataBase*> minpq;
	extern Queue<ExecFuncDataBase*> stg;



	// void* __lp_thr_loop(const uint32 vThrIndex);
	void* __lp_thr_loop(void* vThrIndex);
	void __lp_thr_mng( );

	//Sends a function to an exec queue of the global thread pool
	//When the function will be executed and which queue it will be assigned to depends on its priority
	//A low priority function can be suspended to free a thread and execute one with higher priority
	//*   vFunc: the function to execute
	//*   vPriority: the priority of the function (LUX_PRIORITY_MAX, LUX_PRIORITY_HIGH... )
	//*   pReturn: a pointer to the variable where to store the function return value. Use nullptr for void functions
	//*   pFence: a pointer to a bool variable that will be set to true when the thread returns. Use nullptr if you dont need one
	//*   vParams: the parameters of the function call. Their types must be the same as the function declaration
	//The maximum number of functions executed "at the same time" is defined by LUX_CNF_GLOBAL_THREAD_POOL_SIZE (see LuxEngine_config.h)
	//The actual number of running threads is limited to the number of physical threads in the CPU
	//Use pointers or references to improve performance. The parameters needs to be copied several times before they can be used
	template<class FType, class... PTypes> void sendToExecQueue(FType vFunc, const Priority vPriority/*, FType* pReturn*/, bool* const pFence, PTypes ...vParams) {
		//TODO add return ptr
		ExecFuncData<FType, PTypes...>* execData = new ExecFuncData<FType, PTypes...>;	//Create the function data structure
		execData->func = vFunc;							//Set the function
		//execData->_return = pReturn;					//Set the function
		execData->fence = pFence;						//Set the fence
		execData->params = std::make_tuple(vParams...);	//Set the parameters

		stgAddFence.startSecond( );
		stg.pushFront(execData);						//Assign the data to the staging queue
		stgAddFence.endSecond( );
		pthread_kill(mngThr, SIGCONT);
	}
}