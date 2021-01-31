#pragma once
#define LUX_H_HDCTARRAY
#include "LuxEngine/Types/Integers/Integers.hpp"























namespace lux{
		template<class ...types> struct HdCtArray;
		// Runtime get (rtGet function) helper structures ---------------------------------------------------------------------------------------------//








	namespace __pvt{
		enum __action : uint32{ CHCK = (uint32)-1, DESC = 0, GETV = 1 };								//Enum defining actions for get_t element iterations
		template <uint32 size, __action act, uint32 index, class type, class... types> struct get_t{};	//Unspecialized get_t class. This is used to iterate through the elemenets of the array
		template<uint32 size, uint32 index, class type, class ...types> struct seq;						//seq forward declaration for getArr func_tion

		//CHCK specialization: Checks if the required index is the same as the current one. If true, returns the element. If false, runs another iteration
		template <uint32 size, uint32 index, class type, class... types> struct get_t<size, CHCK, index, type, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return ((seq<size, index, type, types...>*)this)->
				get_t<size, (__action)(getIndex == index), index, type, types...>::template getFunc<getIndex>();
			}
		};

		//DESC specialization: Executes another iteration and calls its CHCK
		template <uint32 size, uint32 index, class type, class... types> struct get_t<size, DESC, index, type, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return ((seq<size, index, type, types...>*)this)->
				seq<size, index - 1, types...>::template get_t<size, CHCK, index - 1, types...>::template getFunc<getIndex>();
			}
		};

		//GETV specialization: Stops iteration and returns the element value
		template <uint32 size, uint32 index, class type, class... types> struct get_t<size, GETV, index, type, types...>{
			template <uint32 getIndex> alwaysInline type &getFunc() {
				return ((seq<size, index, type, types...>*)this)->val;
			}
		};

		#undef genGetVFunc








		// Execute (exec function) helper structures --------------------------------------------------------------------------------------------------//









		struct NoRet_t{};												//Dummy return type for void func_tions
		template<class func_t, class ...args_ts> struct exec_thr{		//Structure containing the func_tion call informations
			func_t _func;												//func_tion pointer
			lux::HdCtArray<args_ts...> _args;							//func_tion arguments
		};


		//Executes a non-void non-member func_tion
		template<class func_t, class ret_t, class ...args_ts> struct exec_t{
			static alwaysInline void exec(func_t _func, ret_t* _ret, args_ts&... _args) { *_ret = _func(_args...); }
		};
		//exec_t specialization. Executes a void non-member func_tion //FIXME REMOVE. merge with automatic return value
		template<class func_t, class ...args_ts> struct exec_t<func_t, NoRet_t, args_ts...> {
			static alwaysInline void exec(func_t _func, NoRet_t* _ret, args_ts&... _args) { _func(_args...); }
		};


		//Executes a non-void member func_tion
		template<class obj_t, class func_t, class ret_t, class ...args_ts> struct execObj_t{
			static alwaysInline void execObj(obj_t& _obj, func_t _func, ret_t* _ret, args_ts&... _args) { *_ret = (_obj.*_func)(_args...); }
		};
		//execObj_t specialization. Executes a void member func_tion //FIXME REMOVE. merge with automatic return value
		template<class obj_t, class func_t, class ...args_ts> struct execObj_t<obj_t, func_t, NoRet_t, args_ts...> {
			static alwaysInline void execObj(obj_t& _obj, func_t _func, NoRet_t* _ret, args_ts&... _args) { (_obj.*_func)(_args...); }
		};








		// seq class (the actual array structure) -----------------------------------------------------------------------------------------------------//








		template<uint32 size, uint32 index, class type, class ...types> struct seq :
		public get_t<size, CHCK, index, type, types...>,
		public get_t<size, DESC, index, type, types...>,
		public get_t<size, GETV, index, type, types...>,
		public seq<size, index - 1, types...>{
			type val;

			//List initialization
			alwaysInline void init(const type& _val, const types&... vals) {
				val = _val; seq<size, index - 1, types...>::init(vals...);
			}

			//Runtime get
			inline void* rtGet(const uint32 _index) {
				return (size - 1 - index == _index) ?
					(void*)&val :
					seq<size, index - 1, types...>::rtGet(_index)
				;
			}

			//Executes a standard func_tion
			template<class func_t, class ret_t, class ...args_ts> alwaysInline void exec(func_t _func, ret_t* _ret, args_ts&... _args) {
				seq<size, index - 1, types...>::template exec<func_t, ret_t, args_ts..., type>(_func, _ret, _args..., val);
			}

			//Executes a member func_tion
			template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void execObj(obj_t& _obj, func_t _func, ret_t* _ret, args_ts&... _args) {
				seq<size, index - 1, types...>::template execObj<obj_t, func_t, ret_t, args_ts...>(_obj, _func, _ret, _args...);
			}
		};








		//Stop at index 0 (seq specialization)
		template<uint32 size, class type> struct seq<size, 0, type> :
		public get_t<size, CHCK, 0, type>,
		public get_t<size, DESC, 0, type>,
		public get_t<size, GETV, 0, type>{
			type val;
			alwaysInline void init(const type& _val) { val = _val; }
			alwaysInline void* rtGet(const uint32 _index) { return (void*)&val; }
			template<class func_t, class ret_t, class ...args_ts> alwaysInline void exec(func_t _func, ret_t* _ret, args_ts&... _args) {
				exec_t<func_t, ret_t, args_ts..., type>::exec(_func, _ret, _args..., val);
			}
			template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void execObj(obj_t& _obj, func_t _func, ret_t* _ret, args_ts&... _args) {
				execObj_t<obj_t, func_t, ret_t, args_ts..., type>::execObj(_obj, _func, _ret, _args..., val);
			}
		};
	}








	// User interface -----------------------------------------------------------------------------------------------------------------------------//







	//Starting index of element iteration. I'm too lazy to write this everywhere
	#define seqIndex (sizeof...(types) - 1)
	/**
	 * @brief "Heterogeneous Data Compile Time Array".
	 *		An array that constains elements of different types.
	 *		Size and types must be known at compile time.
	 *		e.g.  lux::HdCtArray arr = { 1, false, "mogu mogu" };
	 */
	template<class ...types> struct HdCtArray : __pvt::seq<sizeof...(types), seqIndex, types...>{
		alwaysInline HdCtArray() {}
		alwaysInline HdCtArray(types... vals) {
			__pvt::seq<sizeof...(types), seqIndex, types...>::init(vals...);
		}


		/**
		 * @brief Returns a reference to an element.
		 *		This can only be used with compile time known indices.
		 *		Use rtGet<type>(index) to retrieve values in runtime
		 * @tparam vIndex The index of the element
		 */
		template<uint32 vIndex> alwaysInline auto& get() requires(vIndex < sizeof...(types)) {
			// luxDebug(static_assert(vIndex >= 2, "Index is out of range"));
			return __pvt::seq<sizeof...(types), seqIndex, types...>::template get_t<sizeof...(types), __pvt::CHCK, seqIndex, types...>::template getFunc<seqIndex - vIndex>();
		}

		/**
		 * @brief Runtime version of get. Returns a reference to an element.
		 *		Requires the element type to be explicitly specified.   e.g. arr.get<int>(4); //Returns the 4th element as an int&.
		 *		If the type is not specified, the element address is returned as a void*
		 * @param vIndex The index of the element
		 * @tparam eType The type of the element
		 */
		template<class eType> alwaysInline eType& rtGet(const uint32 vIndex) {
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return (eType&)*(eType*)(__pvt::seq<sizeof...(types), seqIndex, types...>::rtGet(vIndex));
		}
		/**
		 * @brief Returns the element address a a void*
		 * @param index The index of the element
		 */
		alwaysInline void* rtGet(const uint32 vIndex) {
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return __pvt::seq<sizeof...(types), seqIndex, types...>::rtGet(vIndex);
		}

		/**
		 * @brief Returns the number of elements in the array
		 */
		alwaysInline constexpr uint32 count() const { return sizeof...(types); }




		//FIXME return the value directly
		/**
		 * @brief Calls a func_tion using the array elements as arguments
		 * @param pFunc: The func_tion to call
		 * @param pReturn: A variable where to store the return value
		 */
		template<class func_t, class ret_t> alwaysInline void exec(func_t pFunc, ret_t& pReturn) {
			__pvt::seq<sizeof...(types), seqIndex, types...>::template exec<func_t, ret_t>(pFunc, &pReturn);
		}
		template<class func_t> alwaysInline void exec(func_t pFunc) {
			__pvt::seq<sizeof...(types), seqIndex, types...>::template exec<func_t, __pvt::NoRet_t>(pFunc, nullptr );
		}




		//pReturn can be omitted to ignore the return value or call void func_tions
		//FIXME return the value directly
		/**
		 * @brief Calls a member func_tion using the array elements as arguments
		 * @param pObject The object to call the func_tion on
		 * @param pFunc The member func_tion to call
		 * @param pReturn A variable where to store the return value
		 */
		template<class obj_t, class func_t, class ret_t> alwaysInline void exec(obj_t& pObject, func_t pFunc, ret_t& pReturn) {
			__pvt::seq<sizeof...(types), seqIndex, types...>::template execObj<obj_t, func_t, ret_t>(pObject, pFunc, &pReturn);
		}
		template<class obj_t, class func_t> alwaysInline void exec(obj_t& pObject, func_t pFunc) {
			__pvt::seq<sizeof...(types), seqIndex, types...>::template execObj<obj_t, func_t, __pvt::NoRet_t>(pObject, pFunc, nullptr );
		}
	};
	#undef seqIndex








	/**
	 * @brief Handy HdCtArray alias
	 */
	template<class... types> struct L : HdCtArray<types...>{
		alwaysInline L() : HdCtArray<types...>() {}
		alwaysInline L(const types&... vals) : HdCtArray<types...>(vals...) {}
	};
}
