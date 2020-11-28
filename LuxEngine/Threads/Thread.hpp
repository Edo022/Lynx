#pragma once
#define LUX_H_THREAD
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "LuxEngine/Types/Containers/HdCtArray.hpp"

//TODO minimize copies
//TODO fix memory leaks

namespace lux{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpointer-arith"
	namespace __pvt{
		template<int n, class funcType, class ...argsTypes> struct thread_ctor_t{
			static void* mt_func(void* _args){
				lux::__pvt::exec_thr<funcType, argsTypes...>* cArgs = (lux::__pvt::exec_thr<funcType, argsTypes...>*)_args;
				cArgs->_args.template exec<funcType>(cArgs->_func);
				return nullptr;
			}
		};
		template<class funcType> struct thread_ctor_t<0, funcType> {
			static void* mt_func(void* _args){
				(*((funcType*)_args))();
				return nullptr;
			}
		};
	}
	struct thread /*: __pvt::thread_ctor_t<1, int>, __pvt::thread_ctor_t<0, int>*/ {
		pthread_t thr;
		//pFunc | The function to initialize the thread with
		//pArgs | List of function arguments
		//e.g. lux::thread t(&sum, {2, 2});
		template<class funcType, class argType, class ...argsTypes> thread(const funcType pFunc, const lux::HdCtArray<argType, argsTypes...>& pArgs) {
			lux::__pvt::exec_thr<funcType, argType, argsTypes...>* func_args = (lux::__pvt::exec_thr<funcType, argType, argsTypes...>*)malloc(sizeof(lux::__pvt::exec_thr<funcType, argsTypes...>));
			func_args->_func = pFunc;
			func_args->_args = pArgs;
			pthread_create(&thr, nullptr, lux::__pvt::thread_ctor_t<1, funcType, argType, argsTypes...>::mt_func, func_args);
		}
		template<class funcType> thread(const funcType pFunc) {
			funcType* func_args = (funcType*)malloc(sizeof(funcType));
			*func_args = pFunc;
			pthread_create(&thr, nullptr, lux::__pvt::thread_ctor_t<0, funcType>::mt_func, func_args);
		}
	#pragma GCC diagnostic pop
		thread(){ thr = 0; }


		//Blocks the execution of a thread. It can be resumed with the resume() function
		inline void suspend(){ pthread_kill(thr, SIGSTOP); }
		//Resumes the execution of a suspended thread. Does nothing if the thread is not suspended
		inline void resume() { pthread_kill(thr, SIGCONT); }


		//Sets the thread name.    this function should only be used for debuggin purposes
		inline void setName(const char* pName){ pthread_setname_np(thr, pName); }
		//Returns the thread name. this function should only be used for debuggin purposes
		inline const char* getName(const char* pName){ char* name; pthread_getname_np(thr, name, 16); return name; }


		inline void join(){ pthread_join(thr, nullptr); }
		inline void detach(){ pthread_detach(thr); }
	};

	namespace thr{ static inline thread thisThread(){ thread thr; thr.thr = pthread_self(); return thr; } }
	// namespace thr{ static inline thread thisThread(){ return thread{.thr = pthread_self()}; } }
};
