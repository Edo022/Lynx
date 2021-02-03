#pragma once
#define LUX_H_THREAD
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include "LuxEngine/Types/Containers/HcArray.hpp"
//TODO minimize copies
//TODO fix memory leaks








namespace lux{
	namespace thr::__pvt{
		template<class func_t, class ...args_ts> struct void_std_args_t{				//Structure containing the function call informations for void functions with not arguments
			func_t _func;															//function pointer
			lux::HcArray<args_ts...> _args;											//function arguments
		};
		
		// template<class func_t> struct void_std_noargs_t{								//Function is directly passed as void*
			// func_t _func;
		// };

		template<class obj_t, class func_t, class ...args_ts> struct void_obj_args_t{	//Structure containing the function call informations for void member functions
			obj_t& _obj;															//function pointer
			func_t _func;															//function pointer
			lux::HcArray<args_ts...> _args;											//function arguments
		};

		template<class obj_t, class func_t> struct void_obj_noargs_t{				//Structure containing the function call informations for void member functions with no arguments
			obj_t& _obj;															//function pointer
			func_t _func;															//function pointer
		};




		//Executes a void function
		template<class func_t, class ...args_ts> static void* run_void_std_args(void* _args) {
			using funcp = void_std_args_t<func_t, args_ts...>*;
			((funcp)_args)->_args.template exec<func_t>(((funcp)_args)->_func);
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing    
		}

		//Executes a void function that takes no arguments
		template<class func_t> alwaysInline static void* run_void_std_noargs(void* _args) {
			((func_t)_args)();			//Cast the function and call it direclty
			return nullptr;				//Return nothing
		}




		//Executes a void member function
		template<class obj_t, class func_t, class ...args_ts> static void* run_void_obj_args(void* _args) {
			using funcp = void_obj_args_t<obj_t, func_t, args_ts...>*;
			((funcp)_args)->_args.template exec<obj_t, func_t>(((funcp)_args)->_obj, ((funcp)_args)->_func);
			delete((funcp)_args);	//Free the function data    //^ Exec HcArray arguments
			return nullptr;			//Return nothing    
		}

		//Executes a void member function that takes no arguments
		template<class obj_t, class func_t> alwaysInline static void* run_void_obj_noargs(void* _args) {
			using funcp = void_obj_noargs_t<obj_t, func_t>*;
			((((funcp)_args)->_obj).*(((funcp)_args)->_func))();	//Cast the object and call its member function
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
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			operator()(pFunc);
		}

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
		template<class func_t, class arg_t, class ...args_ts> alwaysInline void operator()(const func_t pFunc, const L<arg_t, args_ts...>& pArgs)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			using funct = thr::__pvt::void_std_args_t<func_t, arg_t, args_ts...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_std_args<func_t, arg_t, args_ts...>, new funct{ pFunc, pArgs });
		}

		/**
		 * @brief Initializes a thread with a non member void function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class func_t> alwaysInline void operator()(const func_t pFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			pthread_create(&thr, nullptr, thr::__pvt::run_void_std_noargs<func_t>, (void*)pFunc);
		}



		/**
		 * @brief Initializes a thread with a void member function
		 * @param pFunc The function to execute
		 * @param pArgs The function arguments
		 */
		template<class obj_t, class func_t, class arg_t, class ...args_ts> alwaysInline void operator()(obj_t& obj, const func_t pFunc, const L<arg_t, args_ts...>& pArgs)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = thr::__pvt::void_obj_args_t<obj_t, func_t, arg_t, args_ts...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_obj_args<obj_t, func_t, arg_t, args_ts...>, new funct{ obj, pFunc, pArgs });
		}

		/**
		 * @brief Initializes a thread with a void member function that takes no arguments
		 * @param pFunc The function to execute
		 */
		template<class obj_t, class func_t> alwaysInline void operator()(obj_t& obj, const func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = thr::__pvt::void_obj_noargs_t<obj_t, func_t>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_obj_noargs<obj_t, func_t>, new funct{ obj, pFunc });
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
