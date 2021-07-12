#pragma once
#define LNX_H_THREAD_POOL
// #include "Lynx/Lynx_config.hpp"
#include "Lynx/System/System.hpp"
#include "Lynx/System/SystemInfo.hpp"

#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/Types/Containers/RaArray.hpp"
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








namespace lnx::thr {
	//TODO
	enum class Priority : uint16 {
		eMax  = 4,	//execute as soon as possible, eventually suspending the execution of lower priority functions
		eHigh = 3,	//execute only after all the max priority functions have been executed
		eLow  = 2,	//execute only after all the higher priority functions have been executed
		eMin  = 1	//execute when there are no higher priority functions left
	};




	extern RtArray<Thread> threads;
	extern std::deque<ram::ptr<__pvt::Func_b>> queue;
	extern std::mutex queue_m;




	void thrLoop(uint32 vThrIndex);




	/** <pre>
	 * @brief Initializes a thread with a void non member function	\n
	 *     e.g. Thread t(func, P{0.5f})								\n
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 </pre> */
	template<class func_t, class ...args_ts> void runAsync(const func_t vFunc, const P<args_ts...>& pArgs, pollFence& vFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		queue_m.lock();
		using funct = __pvt::void_std_args_xt<func_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &vFence;
		f->_func = vFunc;
		f->_args = pArgs;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}

	/** <pre>
	 * @brief Initializes a thread with a non void non member function	\n
	 *     e.g.															\n
	 *         int ret;													\n
	 *         Thread t(func, P{ 0.5f }, &ret);							\n
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 * @param pRet The address where to store the return value
	 </pre> */
	template<class func_t, class ret_t, class ...args_ts> alwaysInline void runAsync(const func_t vFunc, const P<args_ts...>& pArgs, ret_t* const pRet, pollFence& pFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		queue_m.lock();
		using funct = __pvt::type_std_args_xt<func_t, ret_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_func = vFunc;
		f->_args  = pArgs;
		f->_ret = pRet;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}




	/** <pre>
	 * @brief Initializes a thread with a void non member function that takes no arguments	\n
	 *     e.g. Thread t(func);																\n
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 </pre> */
	template<class func_t> alwaysInline void runAsync(const func_t vFunc, pollFence& pFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		queue_m.lock();
		using funct = __pvt::void_std_noargs_xt<func_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_func = vFunc;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}


	/** <pre>
	 * @brief Initializes a thread with a non void non member function that takes no arguments	\n
	 *     e.g.																					\n
	 *         int ret;																			\n
	 *         Thread t(func, &ret);															\n
	 * Complexity: //TODO
	 * @param vFunc The function to call
	 * @param pRet The address where to store the return value
	 </pre> */
	template<class func_t, class ret_t> alwaysInline void runAsync(const func_t vFunc, ret_t* const pRet, pollFence& pFence)
	requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
		queue_m.lock();
		using funct = __pvt::type_std_noargs_xt<func_t, ret_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_func = vFunc;
		f->_ret = pRet;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}



//FIXME SPECIFY THAT THE TYPES MUST BE AUTOMATICALLY DEDUCED
//FIXME INTERNAL DOCUMENTATION
	/** <pre>
	 * @brief Initializes a thread with a void member function	\n
	 *     e.g.													\n
	 *         Obj obj;											\n
	 *         Thread t(obj, &obj::func, P{ 0.5f });			\n
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 </pre> */
	template<class obj_t, class func_t, class ...args_ts> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, const P<args_ts...>& pArgs, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		queue_m.lock();
		using funct = __pvt::void_obj_args_xt<obj_t, func_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		f->_args = pArgs;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}

	/** <pre>
	 * @brief Initializes a thread with a non void member function	\n
	 *     e.g.														\n
	 *         Obj obj;												\n
	 *         int ret;												\n
	 *         Thread t(obj, &obj::func, P{ 0.5f }, &ret);			\n
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 * @param pArgs A lnx::fwd containing the function arguments
	 * @param pRet The address where to store the return value
	 </pre> */
	template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, const P<args_ts...>& pArgs, ret_t* const pRet, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		queue_m.lock();
		using funct = __pvt::type_obj_args_xt<obj_t, func_t, ret_t, args_ts...>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		f->_args = pArgs;
		f->_ret = pRet;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}




	/** <pre>
	 * @brief Initializes a thread with a void member function that takes no arguments	\n
	 *     e.g.																			\n
	 *         Obj obj;																	\n
	 *         Thread t(obj, &obj::func);												\n
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 </pre> */
	template<class obj_t, class func_t> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		queue_m.lock();
		using funct = __pvt::void_obj_noargs_xt<obj_t, func_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}

	/** <pre>
	 * @brief Initializes a thread with a non void member function that takes no arguments	\n
	 *     e.g.																				\n
	 *         Obj obj;																		\n
	 *         int ret;																		\n
	 *         Thread t(obj, &obj::func, &ret);												\n
	 * Complexity: //TODO
	 * @param pObj The object to call the function on
	 * @param pFunc The address of the member function to call
	 * @param pRet The address where to store the return value
	 </pre> */
	template<class obj_t, class func_t, class ret_t> alwaysInline void runAsync(obj_t& pObj, const func_t pFunc, ret_t* const pRet, pollFence& pFence)
	requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
		queue_m.lock();
		using funct = __pvt::type_obj_noargs_xt<obj_t, func_t, ret_t>;
		ram::ptr<funct> f(sizeof(funct));
		new(f) funct();
		f->_fence = &pFence;
		f->_obj = &pObj;
		f->_func = pFunc;
		f->_ret = pRet;
		queue.push_back((ram::ptr<__pvt::Func_b>)f);
		queue_m.unlock();
	}

}