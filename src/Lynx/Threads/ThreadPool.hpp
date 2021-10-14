#pragma once
#include "Lynx/System/System.hpp"
#include "Lynx/System/SystemInfo.hpp"

#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/Types/Containers/RaArray.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
#include "Lynx/Threads/Thread.hpp"

#include <deque>
#include <mutex>


//TODO ci sono diversi tipi di superfici
//TODO questi tipi vengono calcolati automaticamente e salvati nei dati dell'oggetto
//TODO così in runtime non serve calcolarli. devono essere ricalcolati in background se l'oggetto viene modificato
//TODO ogni superficie ha dei punti caratteristici da cui dipendono le sue proprietà fisiche (non grafica)
//TODO i punti vengono calcolati dopo aver trovato il tipo e sono probabilment indici a dei punti normali
//TODO nn

//TODO add function to send multiple functions to the thread pool to allow them to be execute simultaneously
//TODO while preventing their threads to go in deadlock if synchronized







// namespace lnx{
// 	template<class tType, class tIdxt> struct RtArray;
// }

namespace lnx::thr {
	//TODO
	enum class Priority : uint16 {
		eMax  = 4,	//execute as soon as possible, eventually suspending the execution of lower priority functions
		eHigh = 3,	//execute only after all the max priority functions have been executed
		eLow  = 2,	//execute only after all the higher priority functions have been executed
		eMin  = 1	//execute when there are no higher priority functions left
	};




	_lnx_init_var_value_dec((RtArray<Thread, uint32>), threads);
	_lnx_init_var_value_dec((std::deque<ram::ptr<_pvt::Func_b>>), queue);
	_lnx_init_var_value_dec((std::mutex), queue_m);
	_lnx_init_fun_dec(LNX_H_THREAD_POOL);



	void thrLoop(uint32 vThrIndex);




	/**
	 * @brief Initializes a thread with a void non member function
	 *     e.g. Thread t(func, fwd{0.5f})
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 */
	template<class func_t, class ...args_ts> void runAsync(const func_t vFunc, const fwd<args_ts...>& pArgs, pollFence& vFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		g_queue_m().lock();
		using funct = _pvt::void_std_args_xt<func_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &vFence;
		f->_func = vFunc;
		f->_args = pArgs;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}

	/**
	 * @brief Initializes a thread with a non void non member function
	 *     e.g.
	 *         int ret;
	 *         Thread t(func, fwd{ 0.5f }, &ret);
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 * @param pRet The address where to store the return value
	 */
	template<class func_t, class ret_t, class ...args_ts> alwaysInline void runAsync(const func_t vFunc, const fwd<args_ts...>& pArgs, ret_t* const pRet, pollFence& pFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		g_queue_m().lock();
		using funct = _pvt::type_std_args_xt<func_t, ret_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_func = vFunc;
		f->_args  = pArgs;
		f->_ret = pRet;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}




	/**
	 * @brief Initializes a thread with a void non member function that takes no arguments
	 *     e.g. Thread t(func);
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 */
	template<class func_t> alwaysInline void runAsync(const func_t vFunc, pollFence& pFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		g_queue_m().lock();
		using funct = _pvt::void_std_noargs_xt<func_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_func = vFunc;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}


	/**
	 * @brief Initializes a thread with a non void non member function that takes no arguments
	 *     e.g.
	 *         int ret;
	 *         Thread t(func, &ret);
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 * @param pRet The address where to store the return value
	 */
	template<class func_t, class ret_t> alwaysInline void runAsync(const func_t vFunc, ret_t* const pRet, pollFence& pFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		g_queue_m().lock();
		using funct = _pvt::type_std_noargs_xt<func_t, ret_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_func = vFunc;
		f->_ret = pRet;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}



//FIXME SPECIFY THAT THE TYPES MUST BE AUTOMATICALLY DEDUCED
//FIXME INTERNAL DOCUMENTATION
	/**
	 * @brief Initializes a thread with a void member function
	 *     e.g.
	 *         Obj obj;
	 *         Thread t(obj, &obj::func, fwd{ 0.5f });
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 */
	template<class obj_t, class func_t, class ...args_ts> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, const fwd<args_ts...>& pArgs, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		g_queue_m().lock();
		using funct = _pvt::void_obj_args_xt<obj_t, func_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		f->_args = pArgs;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}

	/**
	 * @brief Initializes a thread with a non void member function
	 *     e.g.
	 *         Obj obj;
	 *         int ret;
	 *         Thread t(obj, &obj::func, fwd{ 0.5f }, &ret);
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 * @param pRet The address where to store the return value
	 */
	template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, const fwd<args_ts...>& pArgs, ret_t* const pRet, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		g_queue_m().lock();
		using funct = _pvt::type_obj_args_xt<obj_t, func_t, ret_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		f->_args = pArgs;
		f->_ret = pRet;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}




	/**
	 * @brief Initializes a thread with a void member function that takes no arguments
	 *     e.g.
	 *         Obj obj;
	 *         Thread t(obj, &obj::func);
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 */
	template<class obj_t, class func_t> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		g_queue_m().lock();
		using funct = _pvt::void_obj_noargs_xt<obj_t, func_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}

	/**
	 * @brief Initializes a thread with a non void member function that takes no arguments
	 *     e.g.
	 *         Obj obj;
	 *         int ret;
	 *         Thread t(obj, &obj::func, &ret);
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 * @param pRet The address where to store the return value
	 */
	template<class obj_t, class func_t, class ret_t> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, ret_t* const pRet, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		g_queue_m().lock();
		using funct = _pvt::type_obj_noargs_xt<obj_t, func_t, ret_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		f->_ret = pRet;
		g_queue().push_back((ram::ptr<_pvt::Func_b>)f);
		g_queue_m().unlock();
	}

}