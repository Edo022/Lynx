#pragma once
#define LNX_H_THREAD
#include "Lynx/Types/Containers/HcArray.hpp"
#include <pthread.h>
#include <csignal>
#include <cstdlib>







namespace lnx{
	struct pollFence;

	namespace thr::__pvt{

		// Functions helper structures ----------------------------------------------------------------------------------------------------------------//








		struct Func_b{
			lnx::pollFence* _fence;
			virtual void exec() = 0;
		};




		//Void standard with arguments
		template<class tFun, class ...tArg> struct void_std_args_xt : public Func_b {
			tFun _func;
			HcArray<tArg...> _args;
			void exec() final { _args.exec(_func); }
		};
		//Type standard with arguments
		template<class tFun, class tRet, class ...tArg> struct type_std_args_xt : public Func_b {
			tFun _func;
			HcArray<tArg...> _args;
			tRet* _ret;
			void exec() final { *_ret = _args.exec(_func); }
		};




		//Void standard with no arguments
		template<class tFun> struct void_std_noargs_xt : public Func_b {
			tFun _func;
			void exec() final { _func(); }
		};
		//Type standard with no arguments
		template<class tFun, class tRet> struct type_std_noargs_xt : public Func_b {
			tFun _func;
			tRet* _ret;
			void exec() final { *_ret = _func(); }
		};




		//Void member with arguments
		template<class tObj, class tFun, class ...tArg> struct void_obj_args_xt : public Func_b {
			tObj* _obj;
			tFun _func;
			HcArray<tArg...> _args;
			void exec() final { _args.execObj(*_obj, _func); }
		};
		//Type member with arguments
		template<class tObj, class tFun, class tRet, class ...tArg> struct type_obj_args_xt : public Func_b {
			tObj* _obj;
			tFun _func;
			HcArray<tArg...> _args;
			tRet* _ret;
			void exec() final { *_ret = _args.execObj(*_obj, _func); }
		};





		//Void member with no arguments
		template<class tObj, class tFun> struct void_obj_noargs_xt : public Func_b {
			tObj* _obj;
			tFun _func;
			void exec() final { (_obj->*_func)(); }
		};
		//Type member with no arguments
		template<class tObj, class tFun, class tRet> struct type_obj_noargs_xt : public Func_b {
			tObj* _obj;
			tFun _func;
			tRet* _ret;
			void exec() final { *_ret = (_obj->*_func)(); }
		};








		//Void standard with arguments
		template<class tFun, class ...tArg> struct void_std_args_t{
			tFun _func;
			HcArray<tArg...> _args;
		};
		//Type standard with arguments
		template<class tFun, class tRet, class ...tArg> struct type_std_args_t {
			tFun _func;
			HcArray<tArg...> _args;
			tRet* _ret;
		};




		//Void standard with no arguments
		/*
		!The function is passed as a void* and doesn't need to be stored in the heap
		template<class tFun> struct void_std_noargs_t{
			tFun _func;
		}; */
		//Type standard with no arguments
		template<class tFun, class tRet> struct type_std_noargs_t {
			tFun _func;
			tRet* _ret;
		};




		//Void member with arguments
		template<class tObj, class tFun, class ...tArg> struct void_obj_args_t{
			tObj& _obj;
			tFun _func;
			HcArray<tArg...> _args;
		};
		//Type member with arguments
		template<class tObj, class tFun, class tRet, class ...tArg> struct type_obj_args_t {
			tObj& _obj;
			tFun _func;
			HcArray<tArg...> _args;
			tRet* _ret;
		};




		//Void member with no arguments
		template<class tObj, class tFun> struct void_obj_noargs_t{
			tObj& _obj;
			tFun _func;
		};
		//Type member with no arguments
		template<class tObj, class tFun, class tRet> struct type_obj_noargs_t {
			tObj& _obj;
			tFun _func;
			tRet* _ret;
		};








		// pthread init functions ---------------------------------------------------------------------------------------------------------------------//








		//Void standard with arguments
		template<class tFun, class ...tArg> static void* run_void_std_args(void* _args) {
			using funcp = void_std_args_t<tFun, tArg...>*;
			((funcp)_args)->_args.template exec<tFun>(((funcp)_args)->_func);
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing
		}
		//Type standard with arguments
		template<class tFun, class tRet, class ...tArg> static void* run_type_std_args(void* _args) {
			using funcp = type_std_args_t<tFun, tRet, tArg...>*;
			*(((funcp)_args)->_ret) = ((funcp)_args)->_args.template exec<tFun>(((funcp)_args)->_func);
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing
		}




		//Void standard with no arguments
		template<class tFun> alwaysInline static void* run_void_std_noargs(void* _args) {
			((tFun)_args)();			//Cast the function and call it direclty
			return nullptr;				//Return nothing
		}
		//Type standard with no arguments
		template<class tFun, class tRet> alwaysInline static void* run_type_std_noargs(void* _args) {
			using funcp = type_std_noargs_t<tFun, tRet>*;
			*(((funcp)_args)->_ret) = (((funcp)_args)->_func)();
			delete((funcp)_args);		//Free the function data    //^ Exec HcArray arguments
			return nullptr;				//Return nothing
		}




		//Void member with arguments
		template<class tObj, class tFun, class ...tArg> static void* run_void_obj_args(void* _args) {
			using funcp = void_obj_args_t<tObj, tFun, tArg...>*;
			((funcp)_args)->_args.template exec<tObj, tFun>(((funcp)_args)->_obj, ((funcp)_args)->_func);
			delete((funcp)_args);	//Free the function data    //^ Exec HcArray arguments
			return nullptr;			//Return nothing
		}
		//Type member with arguments
		template<class tObj, class tFun, class tRet, class ...tArg> static void* run_type_obj_args(void* _args) {
			using funcp = type_obj_args_t<tObj, tFun, tRet, tArg...>*;
			*(((funcp)_args)->_ret) = ((funcp)_args)->_args.template exec<tObj, tFun>(((funcp)_args)->_obj, ((funcp)_args)->_func);
			delete((funcp)_args);	//Free the function data    //^ Exec HcArray arguments
			return nullptr;			//Return nothing
		}




		//Void member with no arguments
		template<class tObj, class tFun> alwaysInline static void* run_void_obj_noargs(void* _args) {
			using funcp = void_obj_noargs_t<tObj, tFun>*;
			((((funcp)_args)->_obj).*(((funcp)_args)->_func))();	//Cast the object and call its member function
			delete((funcp)_args);	//Free the function data
			return nullptr;			//Return nothing
		}
		//Type member with no arguments
		template<class tObj, class tFun, class tRet> alwaysInline static void* run_type_obj_noargs(void* _args) {
			using funcp = type_obj_noargs_t<tObj, tFun, tRet>*;
			*(((funcp)_args)->_ret) = ((((funcp)_args)->_obj).*(((funcp)_args)->_func))();	//Cast the object and call its member function
			delete((funcp)_args);	//Free the function data
			return nullptr;			//Return nothing
		}
	}








	// Helper functions -------------------------------------------------------------------------------------------------------------------------------//








	struct Thread {
		pthread_t thr;



	private:
		template<class tFun, class ...tArg> alwaysInline void dispatch(const tFun vFunc, const P<tArg...>& pArgs)
		requires(std::is_function_v<std::remove_pointer_t<tFun>>) {
			using funct = thr::__pvt::void_std_args_t<tFun, tArg...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_std_args<tFun, tArg...>, new funct{ vFunc, (HcArray<tArg...>)pArgs }); //BUG This uses the template constructor instead of the default copy
		}
		template<class tFun, class tRet, class ...tArg> alwaysInline void dispatch(const tFun vFunc, const P<tArg...>& pArgs, tRet* const pRet)
		requires(std::is_function_v<std::remove_pointer_t<tFun>>) {
			using funct = thr::__pvt::type_std_args_t<tFun, tRet, tArg...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_std_args<tFun, tRet, tArg...>, new funct{ vFunc, (HcArray<tArg...>)pArgs, pRet });
		}


		template<class tFun> alwaysInline void dispatch(const tFun vFunc)
		requires(std::is_function_v<std::remove_pointer_t<tFun>>) {
			pthread_create(&thr, nullptr, thr::__pvt::run_void_std_noargs<tFun>, (void*)vFunc);
		}
		template<class tFun, class tRet> alwaysInline void dispatch(const tFun vFunc, tRet* const pRet)
		requires(std::is_function_v<std::remove_pointer_t<tFun>>) {
			using funct = thr::__pvt::type_std_noargs_t<tFun, tRet>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_std_noargs<tFun, tRet>, new funct{ vFunc, pRet });
		}




		template<class tObj, class tFun, class ...tArg> alwaysInline void dispatch(tObj& pObj, const tFun pFunc, const P<tArg...>& pArgs)
		requires(std::is_object_v<tObj> && std::is_member_function_pointer_v<tFun>) {
			using funct = thr::__pvt::void_obj_args_t<tObj, tFun, tArg...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_obj_args<tObj, tFun, tArg...>, new funct{ pObj, pFunc, (HcArray<tArg...>)pArgs });
		}
		template<class tObj, class tFun, class tRet, class ...tArg> alwaysInline void dispatch(tObj& pObj, const tFun pFunc, const P<tArg...>& pArgs, tRet* const pRet)
		requires(std::is_object_v<tObj> && std::is_member_function_pointer_v<tFun>) {
			using funct = thr::__pvt::type_obj_args_t<tObj, tFun, tRet, tArg...>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_obj_args<tObj, tFun, tRet, tArg...>, new funct{ pObj, pFunc, (HcArray<tArg...>)pArgs, pRet });
		}


		template<class tObj, class tFun> alwaysInline void dispatch(tObj& pObj, const tFun pFunc)
		requires(std::is_object_v<tObj> && std::is_member_function_pointer_v<tFun>) {
			using funct = thr::__pvt::void_obj_noargs_t<tObj, tFun>;
			pthread_create(&thr, nullptr, thr::__pvt::run_void_obj_noargs<tObj, tFun>, new funct{ pObj, pFunc });
		}
		template<class tObj, class tFun, class tRet> alwaysInline void dispatch(tObj& pObj, const tFun pFunc, tRet* const pRet)
		requires(std::is_object_v<tObj> && std::is_member_function_pointer_v<tFun>) {
			using funct = thr::__pvt::type_obj_noargs_t<tObj, tFun, tRet>;
			pthread_create(&thr, nullptr, thr::__pvt::run_type_obj_noargs<tObj, tFun, tRet>, new funct{ pObj, pFunc, pRet });
		}








		// Constructors and operator() ----------------------------------------------------------------------------------------------------------------//








	public:
		//Doesn't initialize the thread. Call operator() on the object to initialize it later
		alwaysInline Thread() { thr = 0; }




		/**
		 * @brief Initializes the thread with a function call
		 *     ! Notice that the @param tags are not actual parameters, as the function takes a variable length template parameter
		 *     ! @param is used to better explain what to pass to this constructor
		 *     Complete signature:
		 *         template<class... tArg> Thread([auto& pObj], auto pFun, [auto* pRet], [P<tArg...> pArg]);
		 *     ! The parameters must always be passed in the shown order, even if some of them are omitted
		 * Cmplexity: O()
		 *
		 * @param pObj The object to call the function on
		 *     This parameter must be omitted if pFun is not a member function
		 * @param pFun The function to start the thread with. This parameter is required. //TODO explain better
		 * @param pRet A pointer that points to a variable where the return value is stored
		 *     The pointer type must match the return type of pFun
		 *     This parameter must be omitted if pFun is void
		 * @param pArg An HcArray (or just P) containing the arguments that will be used to call the function //TODO specify that P is perfect forwarding and not just an htarray
		 *     This parameter must be omitted if pFun has no parameters
		 */
		template<class... tType> alwaysInline Thread(tType&&... pArgs){
			dispatch((std::forward<tType>(pArgs))...);
		}




		/**
		 * @brief Same as constructor. See Thread::Thread
		 */
		template<class... tType> alwaysInline void operator()(tType&&... pArgs){
			dispatch((std::forward<tType>(pArgs))...);
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

			//Returns the calling thread as a lnx::thread structure
			// inline Thread operator()() { Thread thr; thr.thr = pthread_self(); return thr; }
			static inline auto thr() noexcept { return pthread_self(); }
		};
	}
}
