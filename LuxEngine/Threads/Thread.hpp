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
		//Executes a non member void function
		template<int n, class funcType, class ...argsTypes> static void* threadRunVoid(void* _args) {
			auto cArgs = (lux::__pvt::exec_thr<funcType, argsTypes...>*)_args;	//
			cArgs->_args.template exec<funcType>(cArgs->_func);					//
			return nullptr;														//Return nothing
		}


		//Executes a non member void function that takes no arguments
		template<class funcType> alwaysInline static void* threadRunVoidNoParams(void* _args) {
			((funcType)_args)();											//Cast the function to the right type and call it
			return nullptr;														//Return nothing
		}
	}
	#pragma GCC diagnostic pop








	struct Thread {
		pthread_t thr;


		//Doesn't initialize the thread. Call operator() on the object to initialize it later
		alwaysInline Thread() { thr = 0; }

		/**
		 * @brief Initializes a thread with a non member void function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class funcType, class argType, class ...argsTypes> alwaysInline Thread(const funcType pFunc, const lux::HcArray<argType, argsTypes...>& pArgs) {
			operator()(pFunc, pArgs);
		}

		/**
		 * @brief Initializes a thread with a non member void function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class funcType> alwaysInline Thread(const funcType pFunc) { operator()(pFunc); }




		/**
		 * @brief Initializes a thread with a non member void function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class funcType, class argType, class ...argsTypes> void operator()(const funcType pFunc, const lux::HcArray<argType, argsTypes...>& pArgs) {
			auto func_args = (lux::__pvt::exec_thr<funcType, argType, argsTypes...>*)malloc(sizeof(*func_args));
			func_args->_func = pFunc;
			func_args->_args = pArgs;
			pthread_create(&thr, nullptr, lux::__pvt::threadRunVoid<1, funcType, argType, argsTypes...>, func_args);
		}
		/**
		 * @brief Initializes a thread with a non member void function that takes no arguments
		 * @param pFunc The function to execute
		 */
		//FIXME storing a function pointer in a void* is probably not a good idea
		template<class funcType> void operator()(const funcType pFunc) {
			pthread_create(&thr, nullptr, lux::__pvt::threadRunVoidNoParams<funcType>, (void*)pFunc);
		}
		// template<class objType, class funcType, class argType, class ...argsTypes> void operator()(objType& pObj, const funcType pFunc, const lux::HcArray<argType, argsTypes...>& pArgs) {
		// 	lux::__pvt::exec_thr<funcType, argType, argsTypes...>* func_args = (lux::__pvt::exec_thr<funcType, argType, argsTypes...>*)malloc(sizeof(lux::__pvt::exec_thr<funcType, argType, argsTypes...>));
		// 	func_args->_func = pFunc;
		// 	func_args->_args = pArgs;
		// 	pthread_create(&thr, nullptr, lux::__pvt::thread_ctor_t<1, funcType, argType, argsTypes...>::mt_func, func_args);
		// }
		// template<class objType, class funcType> void operator()(objType& pObj, const funcType pFunc) {
		// 	funcType* func_args = (funcType*)malloc(sizeof(funcType));
		// 	*func_args = pFunc;
		// 	pthread_create(&thr, nullptr, lux::__pvt::thread_ctor_t<0, funcType>::mt_func, func_args);
		// }




		//Blocks the execution of a thread. It can be resumed with the resume() function
		inline void suspend() { pthread_kill(thr, SIGSTOP); }
		//Resumes the execution of a suspended thread. Does nothing if the thread is not suspended
		inline void resume() { pthread_kill(thr, SIGCONT); }


		//Sets the thread name.    this function should only be used for debuggin purposes
		inline void setName(const char* pName) { pthread_setname_np(thr, pName); }
		//Returns the thread name. this function should only be used for debuggin purposes
		inline const char* getName() {
			char* name = (char*)malloc(16);
			pthread_getname_np(thr, name, 16);
			return name;
		}


		inline void join() { pthread_join(thr, nullptr); }
		inline void detach() { pthread_detach(thr); }
		inline void yield() { pthread_yield(); }
	};

	namespace thr{
		struct self{
			static inline void suspend() { pthread_kill(pthread_self(), SIGSTOP); }
			static inline void resume() { pthread_kill(pthread_self(), SIGCONT); }

			static inline void setName(const char* pName) {
				pthread_setname_np(pthread_self(), pName);
			}
			static inline const char* getName() {
				char* name = (char*)malloc(16);
				pthread_getname_np(pthread_self(), name, 16);
				return name;
			}

			static inline void detach() { pthread_detach(pthread_self()); }
			static inline void yield() { pthread_yield(); }

			//Returns the calling thread as a lux::thread structure
			inline Thread operator()() { Thread thr; thr.thr = pthread_self(); return thr; }
		};
	}
};
