#pragma once
#define LUX_H_THREAD
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include "LuxEngine/Types/Containers/HcArray.hpp"
//TODO minimize copies
//TODO fix memory leaks








namespace lux{
	namespace __pvt{
		template<class func_t, class ...args_ts> struct exec_void{				//Structure containing the function call informations
			func_t _func;															//function pointer
			lux::HcArray<args_ts...> _args;											//function arguments
		};

		template<class obj_t, class func_t, class ...args_ts> struct exec_obj{	//Structure containing the function call informations for member functions
			obj_t& _obj;															//function pointer
			func_t _func;															//function pointer
			lux::HcArray<args_ts...> _args;											//function arguments
		};

		template<class obj_t, class func_t> struct exec_void_obj{	//Structure containing the function call informations for member functions
			obj_t& _obj;															//function pointer
			func_t _func;															//function pointer
		};




		//Executes a non member void function
		template<class func_t, class ...args_ts> static void* threadRunVoid(void* _args) {
			using funcp = exec_void<func_t, args_ts...>*;
			((funcp)_args)->_args.template exec<func_t>(((funcp)_args)->_func);
			delete((funcp)_args);		//Free the function data
			return nullptr;			//Return nothing    //^ Cast to thr struct and call the function with its parameters
		}

		//Executes a non member void function that takes no arguments
		template<class func_t> alwaysInline static void* threadRunVoidNoParams(void* _args) {
			((func_t)_args)();	//Cast the function to the right type and call it
			return nullptr;			//Return nothing
		}




		//Executes a member void function
		template<class obj_t, class func_t, class ...args_ts> static void* threadRunObjVoid(void* _args) {
			using funcp = exec_obj<obj_t, func_t, args_ts...>*;
			((funcp)_args)->_args.template exec<obj_t, func_t>(((funcp)_args)->_obj, ((funcp)_args)->_func);
			delete((funcp)_args);	//Free the function data
			return nullptr;			//Return nothing    //^ Cast to thr struct and call the function with its parameters
		}

		//Executes a member void function that takes no arguments
		template<class obj_t, class func_t> alwaysInline static void* threadRunObjVoidNoParams(void* _args) {
			using funcp = exec_void_obj<obj_t, func_t>*;
			((((funcp)_args)->_obj).*(((funcp)_args)->_func))();	//Cast the function to the right type and call it
			delete((funcp)_args);	//Free the function data
			return nullptr;			//Return nothing
		}
	}








	// Thread struct ----------------------------------------------------------------------------------------------------------------------------------//








	struct Thread {
		pthread_t thr;




		// Constructors -------------------------------------------------------------------------------------------------------------------------------//




		//Doesn't initialize the thread. Call operator() on the object to initialize it later
		alwaysInline Thread() { thr = 0; }

		/**
		 * @brief Initializes a thread with a non member void function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class func_t, class arg_t, class ...args_ts> alwaysInline Thread(const func_t pFunc, const L<arg_t, args_ts...>& pArgs)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			operator()(pFunc, pArgs);
		}

		/**
		 * @brief Initializes a thread with a non member void function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class func_t> alwaysInline Thread(const func_t pFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) { operator()(pFunc); }

		/**
		 * @brief Initializes a thread with a void member function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class obj_t, class func_t, class arg_t, class ...args_ts> alwaysInline Thread(obj_t& obj, const func_t pFunc, const L<arg_t, args_ts...>& pArgs)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			operator()(obj, pFunc, pArgs);
		}

		/**
		 * @brief Initializes a thread with a void member function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class obj_t, class func_t> alwaysInline Thread(obj_t& obj, const func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			operator()(obj, pFunc);
		}


		// Operator() ---------------------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Initializes a thread with a non member void function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class func_t, class arg_t, class ...args_ts> void operator()(const func_t pFunc, const L<arg_t, args_ts...>& pArgs)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			using funct = lux::__pvt::exec_void<func_t, arg_t, args_ts...>;
			auto funcd = (funct*)malloc(sizeof(funct));		//Allocate function data in the heap so that it doesnt get destroyed when the parent returns
			funcd->_func = pFunc;							//Copy function address
			funcd->_args = pArgs;							//Copy (((parameters references) array) by value)
			pthread_create(&thr, nullptr, lux::__pvt::threadRunVoid<func_t, arg_t, args_ts...>, funcd);
		}

		/**
		 * @brief Initializes a thread with a non member void function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class func_t> void operator()(const func_t pFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			pthread_create(&thr, nullptr, lux::__pvt::threadRunVoidNoParams<func_t>, (void*)pFunc);
		}



		/**
		 * @brief Initializes a thread with a void member function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class obj_t, class func_t, class arg_t, class ...args_ts> void operator()(obj_t& obj, const func_t pFunc, const L<arg_t, args_ts...>& pArgs)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = lux::__pvt::exec_obj<obj_t, func_t, arg_t, args_ts...>;
			auto funcd = new funct{
				._obj = obj,
				._func = pFunc,
				._args = pArgs
			};
			pthread_create(&thr, nullptr, lux::__pvt::threadRunObjVoid<obj_t, func_t, arg_t, args_ts...>, funcd);
		}

		/**
		 * @brief Initializes a thread with a void member function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class obj_t, class func_t> void operator()(obj_t& obj, const func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = lux::__pvt::exec_void_obj<obj_t, func_t>;
			auto funcd = new funct{
				._obj = obj,
				._func = pFunc
			};
			pthread_create(&thr, nullptr, lux::__pvt::threadRunObjVoidNoParams<obj_t, func_t>, funcd);
		}




		// Utilities ----------------------------------------------------------------------------------------------------------------------------------//




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








	// Self -------------------------------------------------------------------------------------------------------------------------------------------//








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
