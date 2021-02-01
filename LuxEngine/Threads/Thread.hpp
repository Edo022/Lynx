#pragma once
#define LUX_H_THREAD
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include "LuxEngine/Types/Containers/HcArray.hpp"

//TODO minimize copies
//TODO fix memory leaks


namespace lux{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpointer-arith"
	namespace __pvt{
		template<int n, class funcType, class ...argsTypes> struct thread_ctor_t{
			static void* mt_func(void* _args) {
				lux::__pvt::exec_thr<funcType, argsTypes...>* cArgs = (lux::__pvt::exec_thr<funcType, argsTypes...>*)_args;
				cArgs->_args.template exec<funcType>(cArgs->_func);
				return nullptr;
			}
		};
		template<class funcType> struct thread_ctor_t<0, funcType> {
			static void* mt_func(void* _args) {
				(*((funcType*)_args))();
				return nullptr;
			}
		};
	}
	#pragma GCC diagnostic pop
	struct thread /*: __pvt::thread_ctor_t<1, int>, __pvt::thread_ctor_t<0, int>*/ {
		pthread_t thr;
		//pFunc | The function to initialize the thread with
		//pArgs | List of function arguments
		//e.g. lux::thread t(&sum, {2, 2});
		template<class funcType, class argType, class ...argsTypes> inline thread(const funcType pFunc, const lux::HcArray<argType, argsTypes...>& pArgs) {
			operator()(pFunc, pArgs);
		}
		template<class funcType> inline thread(const funcType pFunc) { operator()(pFunc); }
		thread() { thr = 0; }




		template<class funcType, class argType, class ...argsTypes> void operator()(const funcType pFunc, const lux::HcArray<argType, argsTypes...>& pArgs) {
			lux::__pvt::exec_thr<funcType, argType, argsTypes...>* func_args = (lux::__pvt::exec_thr<funcType, argType, argsTypes...>*)malloc(sizeof(lux::__pvt::exec_thr<funcType, argType, argsTypes...>));
			func_args->_func = pFunc;
			func_args->_args = pArgs;
			pthread_create(&thr, nullptr, lux::__pvt::thread_ctor_t<1, funcType, argType, argsTypes...>::mt_func, func_args);
		}
		template<class funcType> void operator()(const funcType pFunc) {
			funcType* func_args = (funcType*)malloc(sizeof(funcType));
			*func_args = pFunc;
			pthread_create(&thr, nullptr, lux::__pvt::thread_ctor_t<0, funcType>::mt_func, func_args);
		}




		//Blocks the execution of a thread. It can be resumed with the resume() function
		inline void suspend() { pthread_kill(thr, SIGSTOP); }
		//Resumes the execution of a suspended thread. Does nothing if the thread is not suspended
		inline void resume() { pthread_kill(thr, SIGCONT); }


		//Sets the thread name.    this function should only be used for debuggin purposes
		inline void setName(const char* pName) { pthread_setname_np(thr, pName); }
		//Returns the thread name. this function should only be used for debuggin purposes
		inline const char* getName(const char* pName) { char* name; pthread_getname_np(thr, name, 16); return name; }


		inline void join() { pthread_join(thr, nullptr); }
		inline void detach() { pthread_detach(thr); }
		inline void yield() { pthread_yield(); }
	};

	namespace thr{
		struct self{
			static inline void suspend() { pthread_kill(pthread_self(), SIGSTOP); }
			static inline void resume() { pthread_kill(pthread_self(), SIGCONT); }

			static inline void setName(const char* pName) { pthread_setname_np(pthread_self(), pName); }
			static inline const char* getName(const char* pName) { char* name; pthread_getname_np(pthread_self(), name, 16); return name; }

			static inline void detach() { pthread_detach(pthread_self()); }
			static inline void yield() { pthread_yield(); }

			//Returns the calling thread as a lux::thread structure
			inline thread operator()() { thread thr; thr.thr = pthread_self(); return thr; }
		};
	}
};
