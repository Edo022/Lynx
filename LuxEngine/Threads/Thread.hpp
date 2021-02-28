#pragma once
#define LUX_H_THREAD
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include "LuxEngine/Types/Containers/HcArray.hpp"
// #include "LuxEngine/Types/LuxFenceDE.hpp"







namespace lux{
	struct pollFence;

	namespace thr::__pvt{

		// Functions helper structures ----------------------------------------------------------------------------------------------------------------//








		struct Func_b{
			lux::pollFence* _fence;
			virtual void exec() = 0;
		};




		//Void standard with arguments
		template<class func_t, class ...args_ts> struct void_std_args_xt : public Func_b {
			func_t _func;
			lux::HcArray<args_ts...> _args;
			void exec() final override { _args.exec(_func); }
		};
		//Type standard with arguments
		template<class func_t, class ret_t, class ...args_ts> struct type_std_args_xt : public Func_b {
			func_t _func;
			lux::HcArray<args_ts...> _args;
			ret_t* _ret;
			void exec() final override { *_ret = _args.exec(_func); }
		};




		//Void standard with no arguments
		template<class func_t> struct void_std_noargs_xt : public Func_b {
			func_t _func;
			void exec() final override { _func(); }
		};
		//Type standard with no arguments
		template<class func_t, class ret_t> struct type_std_noargs_xt : public Func_b {
			func_t _func;
			ret_t* _ret;
			void exec() final override { *_ret = _func(); }
		};




		//Void member with arguments
		template<class obj_t, class func_t, class ...args_ts> struct void_obj_args_xt : public Func_b {
			obj_t* _obj;
			func_t _func;
			lux::HcArray<args_ts...> _args;
			void exec() final override { _args.execObj(*_obj, _func); }
		};
		//Type member with arguments
		template<class obj_t, class func_t, class ret_t, class ...args_ts> struct type_obj_args_xt : public Func_b {
			obj_t* _obj;
			func_t _func;
			lux::HcArray<args_ts...> _args;
			ret_t* _ret;
			void exec() final override { *_ret = _args.execObj(*_obj, _func); }
		};





		//Void member with no arguments
		template<class obj_t, class func_t> struct void_obj_noargs_xt : public Func_b {
			obj_t* _obj;
			func_t _func;
			void exec() final override { (_obj->*_func)(); }
		};
		//Type member with no arguments
		template<class obj_t, class func_t, class ret_t> struct type_obj_noargs_xt : public Func_b {
			obj_t* _obj;
			func_t _func;
			ret_t* _ret;
			void exec() final override { *_ret = (_obj->*_func)(); }
		};































		//Void standard with arguments
		template<class func_t, class ...args_ts> struct void_std_args_t{
			func_t _func;
			lux::HcArray<args_ts...> _args;
		};
		//Type standard with arguments
		template<class func_t, class ret_t, class ...args_ts> struct type_std_args_t {
			func_t _func;
			lux::HcArray<args_ts...> _args;
			ret_t* _ret;
		};




		//Void standard with no arguments
		/*
		!The function is passed as a void* and doesn't need to be stored in the heap
		template<class func_t> struct void_std_noargs_t{
			func_t _func;
		}; */
		//Type standard with no arguments
		template<class func_t, class ret_t> struct type_std_noargs_t {
			func_t _func;
			ret_t* _ret;
		};




		//Void member with arguments
		template<class obj_t, class func_t, class ...args_ts> struct void_obj_args_t{
			obj_t& _obj;
			func_t _func;
			lux::HcArray<args_ts...> _args;
		};
		//Type member with arguments
		template<class obj_t, class func_t, class ret_t, class ...args_ts> struct type_obj_args_t {
			obj_t& _obj;
			func_t _func;
			lux::HcArray<args_ts...> _args;
			ret_t* _ret;
		};




		//Void member with no arguments
		template<class obj_t, class func_t> struct void_obj_noargs_t{
			obj_t& _obj;
			func_t _func;
		};
		//Type member with no arguments
		template<class obj_t, class func_t, class ret_t> struct type_obj_noargs_t {
			obj_t& _obj;
			func_t _func;
			ret_t* _ret;
		};








		// pthread init functions ---------------------------------------------------------------------------------------------------------------------//








		//Void standard with arguments
		template<class func_t, class ...args_ts> static void* run_void_std_args(void* _args) {
			using funcp = void_std_args_t<func_t, args_ts...>*;
			((funcp)_args)->_args.template exec<func_t>(((funcp)_args)->_func);
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing
		}
		//Type standard with arguments
		template<class func_t, class ret_t, class ...args_ts> static void* run_type_std_args(void* _args) {
			using funcp = type_std_args_t<func_t, ret_t, args_ts...>*;
			*(((funcp)_args)->_ret) = ((funcp)_args)->_args.template exec<func_t>(((funcp)_args)->_func);
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing
		}




		//Void standard with no arguments
		template<class func_t> alwaysInline static void* run_void_std_noargs(void* _args) {
			((func_t)_args)();			//Cast the function and call it direclty
			return nullptr;				//Return nothing
		}
		//Type standard with no arguments
		template<class func_t, class ret_t> alwaysInline static void* run_type_std_noargs(void* _args) {
			using funcp = type_std_noargs_t<func_t, ret_t>*;
			*(((funcp)_args)->_ret) = (((funcp)_args)->_func)();
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing
		}




		//Void member with arguments
		template<class obj_t, class func_t, class ...args_ts> static void* run_void_obj_args(void* _args) {
			using funcp = void_obj_args_t<obj_t, func_t, args_ts...>*;
			((funcp)_args)->_args.template exec<obj_t, func_t>(((funcp)_args)->_obj, ((funcp)_args)->_func);
			delete((funcp)_args);	//Free the function data    //^ Exec HcArray arguments
			return nullptr;			//Return nothing
		}
		//Type member with arguments
		template<class obj_t, class func_t, class ret_t, class ...args_ts> static void* run_type_obj_args(void* _args) {
			using funcp = type_obj_args_t<obj_t, func_t, ret_t, args_ts...>*;
			*(((funcp)_args)->_ret) = ((funcp)_args)->_args.template exec<obj_t, func_t>(((funcp)_args)->_obj, ((funcp)_args)->_func);
			delete((funcp)_args);	//Free the function data    //^ Exec HcArray arguments
			return nullptr;			//Return nothing
		}




		//Void member with no arguments
		template<class obj_t, class func_t> alwaysInline static void* run_void_obj_noargs(void* _args) {
			using funcp = void_obj_noargs_t<obj_t, func_t>*;
			((((funcp)_args)->_obj).*(((funcp)_args)->_func))();	//Cast the object and call its member function
			delete((funcp)_args);	//Free the function data
			return nullptr;			//Return nothing
		}
		//Type member with no arguments
		template<class obj_t, class func_t, class ret_t> alwaysInline static void* run_type_obj_noargs(void* _args) {
			using funcp = type_obj_noargs_t<obj_t, func_t, ret_t>*;
			*(((funcp)_args)->_ret) = ((((funcp)_args)->_obj).*(((funcp)_args)->_func))();	//Cast the object and call its member function
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
		 * @brief Initializes a thread with a void non member function.
		 *		e.g. --- Thread t(func, L{ 0.5f }); ---
		 * @param vFunc The function to call
		 * @param pArgs An HcArray containing the function arguments
		 */
		template<class func_t, class ...args_ts> alwaysInline Thread(const func_t vFunc, const L<args_ts...>& pArgs)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			operator()(vFunc, pArgs);
		}

		/**
		 * @brief Initializes a thread with a non void non member function.
		 *		e.g. --- int ret; Thread t(func, L{ 0.5f }, &ret); ---
		 * @param vFunc The function to call
		 * @param pArgs An HcArray containing the function arguments
		 * @param pRet The address where to store the return value
		 */
		template<class func_t, class ret_t, class ...args_ts> alwaysInline Thread(const func_t vFunc, const L<args_ts...>& pArgs, ret_t* const pRet)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			operator()(vFunc, pArgs, pRet);
		}




		/**
		 * @brief Initializes a thread with a void non member function that takes no arguments.
		 *		e.g. --- Thread t(func); ---
		 * @param vFunc The function to call
		 */
		template<class func_t> alwaysInline Thread(const func_t vFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			operator()(vFunc);
		}

		/**
		 * @brief Initializes a thread with a non void non member function that takes no arguments.
		 *		e.g. --- int ret; Thread t(func, &ret); ---
		 * @param vFunc The function to call
		 * @param pRet The address where to store the return value
		 */
		template<class func_t, class ret_t> alwaysInline Thread(const func_t vFunc, ret_t* const pRet)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			operator()(vFunc, pRet);
		}




		/**
		 * @brief Initializes a thread with a void member function
		 *		e.g. --- Obj obj; Thread t(obj, &obj::func, L{ 0.5f }); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 * @param pArgs An HcArray containing the function arguments
		 */
		template<class obj_t, class func_t, class ...args_ts> alwaysInline Thread(obj_t& pObj, const func_t pFunc, const L<args_ts...>& pArgs)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			operator()(pObj, pFunc, pArgs);
		}

		/**
		 * @brief Initializes a thread with a non void member function.
		 *		e.g. --- Obj obj; int ret; Thread t(obj, &obj::func, L{ 0.5f }, &ret); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 * @param pArgs An HcArray containing the function arguments
		 * @param pRet The address where to store the return value
		 */
		template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline Thread(obj_t& pObj, const func_t pFunc, const L<args_ts...>& pArgs, ret_t* const pRet)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			operator()(pObj, pFunc, pArgs, pRet);
		}




		/**
		 * @brief Initializes a thread with a void member function that takes no arguments.
		 *		e.g. --- Obj obj; Thread t(obj, &obj::func); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 */
		template<class obj_t, class func_t> alwaysInline Thread(obj_t& pObj, const func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			operator()(pObj, pFunc);
		}

		/**
		 * @brief Initializes a thread with a non void member function that takes no arguments.
		 *		e.g. --- Obj obj; int ret; Thread t(obj, &obj::func, &ret); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 * @param pRet The address where to store the return value
		 */
		template<class obj_t, class func_t, class ret_t> alwaysInline Thread(obj_t& pObj, const func_t pFunc, ret_t* const pRet)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			operator()(pObj, pFunc, pRet);
		}








		// Operator() ---------------------------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief Initializes a thread with a void non member function.
		 *		e.g. --- Thread t(func, L{ 0.5f }); ---
		 * @param vFunc The function to call
		 * @param pArgs An HcArray containing the function arguments
		 */
		template<class func_t, class ...args_ts> alwaysInline void operator()(const func_t vFunc, const L<args_ts...>& pArgs)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			using funct = thr::__pvt::void_std_args_t<func_t, args_ts...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_std_args<func_t, args_ts...>, new funct{ vFunc, pArgs });
		}

		/**
		 * @brief Initializes a thread with a non void non member function.
		 *		e.g. --- int ret; Thread t(func, L{ 0.5f }, &ret); ---
		 * @param vFunc The function to call
		 * @param pArgs An HcArray containing the function arguments
		 * @param pRet The address where to store the return value
		 */
		template<class func_t, class ret_t, class ...args_ts> alwaysInline void operator()(const func_t vFunc, const L<args_ts...>& pArgs, ret_t* const pRet)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			using funct = thr::__pvt::type_std_args_t<func_t, ret_t, args_ts...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_std_args<func_t, ret_t, args_ts...>, new funct{ vFunc, pArgs, pRet });
		}




		/**
		 * @brief Initializes a thread with a void non member function that takes no arguments.
		 *		e.g. --- Thread t(func); ---
		 * @param vFunc The function to call
		 */
		template<class func_t> alwaysInline void operator()(const func_t vFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			pthread_create(&thr, nullptr, thr::__pvt::run_void_std_noargs<func_t>, (void*)vFunc);
		}

		/**
		 * @brief Initializes a thread with a non void non member function that takes no arguments.
		 *		e.g. --- int ret; Thread t(func, &ret); ---
		 * @param vFunc The function to call
		 * @param pRet The address where to store the return value
		 */
		template<class func_t, class ret_t> alwaysInline void operator()(const func_t vFunc, ret_t* const pRet)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			using funct = thr::__pvt::type_std_noargs_t<func_t, ret_t>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_std_noargs<func_t, ret_t>, new funct{ vFunc, pRet });
		}




		/**
		 * @brief Initializes a thread with a void member function
		 *		e.g. --- Obj obj; Thread t(obj, &obj::func, L{ 0.5f }); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 * @param pArgs An HcArray containing the function arguments
		 */
		template<class obj_t, class func_t, class ...args_ts> alwaysInline void operator()(obj_t& pObj, const func_t pFunc, const L<args_ts...>& pArgs)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = thr::__pvt::void_obj_args_t<obj_t, func_t, args_ts...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_obj_args<obj_t, func_t, args_ts...>, new funct{ pObj, pFunc, pArgs });
		}

		/**
		 * @brief Initializes a thread with a non void member function.
		 *		e.g. --- Obj obj; int ret; Thread t(obj, &obj::func, L{ 0.5f }, &ret); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 * @param pArgs An HcArray containing the function arguments
		 * @param pRet The address where to store the return value
		 */
		template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void operator()(obj_t& pObj, const func_t pFunc, const L<args_ts...>& pArgs, ret_t* const pRet)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = thr::__pvt::type_obj_args_t<obj_t, func_t, ret_t, args_ts...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_obj_args<obj_t, func_t, ret_t, args_ts...>, new funct{ pObj, pFunc, pArgs, pRet });
		}




		/**
		 * @brief Initializes a thread with a void member function that takes no arguments.
		 *		e.g. --- Obj obj; Thread t(obj, &obj::func); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 */
		template<class obj_t, class func_t> alwaysInline void operator()(obj_t& pObj, const func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = thr::__pvt::void_obj_noargs_t<obj_t, func_t>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_obj_noargs<obj_t, func_t>, new funct{ pObj, pFunc });
		}

		/**
		 * @brief Initializes a thread with a non void member function that takes no arguments.
		 *		e.g. --- Obj obj; int ret; Thread t(obj, &obj::func, &ret); ---
		 * @param pObj The object to call the function on
		 * @param pFunc The address of the member function to call
		 * @param pRet The address where to store the return value
		 */
		template<class obj_t, class func_t, class ret_t> alwaysInline void operator()(obj_t& pObj, const func_t pFunc, ret_t* const pRet)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			using funct = thr::__pvt::type_obj_noargs_t<obj_t, func_t, ret_t>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_obj_noargs<obj_t, func_t, ret_t>, new funct{ pObj, pFunc, pRet });
		}








		// Utilities ----------------------------------------------------------------------------------------------------------------------------------//








		//Blocks the execution of a thread. It can be resumed with the resume() function
		inline void suspend() { pthread_kill(thr, SIGSTOP); }
		//Resumes the execution of a suspended thread. Does nothing if the thread is not suspended
		inline void  resume() { pthread_kill(thr, SIGCONT); }


		//Sets the thread name.    this function should only be used for debugging purposes
		inline void setName(const char* pName) { pthread_setname_np(thr, pName); }
		//Returns the thread name. this function should only be used for debugging purposes
		inline const char* getName() {
			char* name = (char*)malloc(16);
			pthread_getname_np(thr, name, 16);
			return name;
		}


		//pthread_join   wrapper
		inline void   join() { pthread_join(thr, nullptr); }
		//pthread_detach wrapper
		inline void detach() { pthread_detach(thr); }
	};








	// Self -------------------------------------------------------------------------------------------------------------------------------------------//








	namespace thr{
		struct self{
			static inline void suspend() { pthread_kill(pthread_self(), SIGSTOP); }
			static inline void  resume() { pthread_kill(pthread_self(), SIGCONT); }

			static inline void setName(const char* pName) {
				pthread_setname_np(pthread_self(), pName);
			}
			static inline const char* getName() {
				char* name = (char*)malloc(16);
				pthread_getname_np(pthread_self(), name, 16);
				return name;
			}

			//pthread_yield wrapper
			static inline void yield() { pthread_yield(); }

			//Returns the calling thread as a lux::thread structure
			inline Thread operator()() { Thread thr; thr.thr = pthread_self(); return thr; }
		};
	}
}
