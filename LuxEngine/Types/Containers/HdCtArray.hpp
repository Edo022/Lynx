#define LUX_H_HDCTARRAY
#include "LuxEngine/Types/Integers/Integers.hpp"























namespace lux{
	template<class ...types> struct HdCtArray;


		// Runtime get (operator[]) helper structures -------------------------------------------------------------------------------------------------//








	namespace __pvt{
		enum __action : uint32{ CHCK = (uint32)-1, DESC = 0, GETV = 1 };					//Enum defining actions for get_t element iterations
		template <__action act, uint32 index, class type, class... types> struct get_t{ };	//Unspecialized get_t class. This is used to iterate through the elemenets of the array
		template<uint32 index, class type, class ...types> struct seq;						//seq forward declaration for getArr func_tion
		#define genGetVFunc alwaysInline virtual seq<index, type, types...>* getArr() = 0;		//getArr func_tion shared by all the get_t specializations. Returns the HdCtArray object address

		//CHCK specialization: Checks if the required index is the same as the current one. If true, returns the element. If false, runs another iteration
		template <uint32 index, class type, class... types> struct get_t<CHCK, index, type, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return getArr()->get_t<(__action)(getIndex == index), index, type, types...>::template getFunc<getIndex>();
			}
			genGetVFunc;
		};

		//DESC specialization: Executes another iteration and calls its CHCK
		template <uint32 index, class type, class... types> struct get_t<DESC, index, type, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return getArr()->seq<index - 1, types...>::template get_t<CHCK, index - 1, types...>::template getFunc<getIndex>();
			}
			genGetVFunc;
		};

		//GETV specialization: Stops iteration and returns the element value
		template <uint32 index, class type, class... types> struct get_t<GETV, index, type, types...>{
			template <uint32 getIndex> alwaysInline type &getFunc() {
				return getArr()->val;
			}
			genGetVFunc;
		};

		#undef genGetVFunc








		// Execute (exec func_tion) helper structures --------------------------------------------------------------------------------------------------//









		struct NoRet_t{};											//Dummy return type for void func_tions
		template<class func_t, class ...args_ts> struct exec_thr{		//Structure containing the func_tion call informations
			func_t _func;												//func_tion pointer
			lux::HdCtArray<args_ts...> _args;							//func_tion arguments
		};


		//Executes a non-void non-member func_tion
		template<class func_t, class ret_t, class ...args_ts> struct exec_t{
			static alwaysInline void exec(func_t _func, ret_t* _ret, args_ts&... _args){ *_ret = _func(_args...); }
		};
		//exec_t specialization. Executes a void non-member func_tion //FIXME REMOVE. merge with automatic return value
		template<class func_t, class ...args_ts> struct exec_t<func_t, NoRet_t, args_ts...> {
			static alwaysInline void exec(func_t _func, NoRet_t* _ret, args_ts&... _args){ _func(_args...); }
		};


		//Executes a non-void member func_tion
		template<class obj_t, class func_t, class ret_t, class ...args_ts> struct execObj_t{
			static alwaysInline void execObj(obj_t& _obj, func_t _func, ret_t* _ret, args_ts&... _args){ *_ret = (_obj.*_func)(_args...); }
		};
		//execObj_t specialization. Executes a void member func_tion //FIXME REMOVE. merge with automatic return value
		template<class obj_t, class func_t, class ...args_ts> struct execObj_t<obj_t, func_t, NoRet_t, args_ts...> {
			static alwaysInline void execObj(obj_t& _obj, func_t _func, NoRet_t* _ret, args_ts&... _args){ (_obj.*_func)(_args...); }
		};








		// seq class (the actual array structure) -----------------------------------------------------------------------------------------------------//








		template<uint32 index, class type, class ...types> struct seq :
		public get_t<CHCK, index, type, types...>,
		public get_t<DESC, index, type, types...>,
		public get_t<GETV, index, type, types...>,
		public seq<index - 1, types...>{
			type val;
			alwaysInline virtual constexpr uint64 getOriginalSize() = 0;							//Returns the array size. Overridden by HdCtArray
			alwaysInline virtual seq<index, type, types...>* getArr() override { return this; }		//USed by get_t structures

			//List initialization
			alwaysInline void init(const type& _val, const types&... vals){
				val = _val; seq<index - 1, types...>::init(vals...);
			}

			//Runtime get
			inline void* rtGet(const uint32 _index){
				return (getOriginalSize() - 1 - index == _index) ?
					(void*)&val :
					seq<index - 1, types...>::rtGet(_index)
				;
			}

			//Executes a standard func_tion
			template<class func_t, class ret_t, class ...args_ts> alwaysInline void exec(func_t _func, ret_t* _ret, args_ts&... _args){
				seq<index - 1, types...>::template exec<func_t, ret_t, args_ts..., type>(_func, _ret, _args..., val);
			}

			//Executes a member func_tion
			template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void execObj(obj_t& _obj, func_t _func, ret_t* _ret, args_ts&... _args){
				seq<index - 1, types...>::template execObj<obj_t, func_t, ret_t, args_ts...>(_obj, _func, _ret, _args...);
			}

			//func_t:  func_tion type
			//ret_t:  Return type
			//args_ts: func_tion arguments types
			//obj_t:  Object type
		};








		//Stop at index 0 (seq specialization)
		template<class type> struct seq<0, type> :
		public get_t<CHCK, 0, type>,
		public get_t<DESC, 0, type>,
		public get_t<GETV, 0, type>{
			type val;
			alwaysInline virtual seq<0, type>* getArr() override { return this; }
			alwaysInline void init(const type& _val){ val = _val; }
			alwaysInline void* rtGet(const uint32 _index){ return (void*)&val; }
			template<class func_t, class ret_t, class ...args_ts> alwaysInline void exec(func_t _func, ret_t* _ret, args_ts&... _args){
				exec_t<func_t, ret_t, args_ts..., type>::exec(_func, _ret, _args..., val);
			}
			template<class obj_t, class func_t, class ret_t, class ...args_ts> alwaysInline void execObj(obj_t& _obj, func_t _func, ret_t* _ret, args_ts&... _args){
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
	template<class ...types> struct HdCtArray : __pvt::seq<seqIndex, types...>{
		alwaysInline HdCtArray(){}
		alwaysInline HdCtArray(types... vals){
			__pvt::seq<seqIndex, types...>::init(vals...);
		}
		alwaysInline virtual uint64 getOriginalSize(){ return count(); } //FIXME move to template


		/**
		 * @brief Returns a reference to an element.
		 *		This can only be used with compile time known indices.
		 *		Use rtGet<type>(index) to retrieve values in runtime
		 * @tparam vIndex The index of the element
		 */
		template<uint32 vIndex> alwaysInline auto& get(){
			return __pvt::seq<seqIndex, types...>::template get_t<__pvt::CHCK, seqIndex, types...>::template getFunc<seqIndex - vIndex>();
		}

		/**
		 * @brief Runtime version of get. Returns a reference to an element.
		 *		Requires the element type to be explicitly specified.   e.g. arr.get<int>(4); //Returns the 4th element as an int&.
		 *		If the type is not specified, the element address is returned as a void*
		 * @param vIndex The index of the element
		 * @tparam eType The type of the element
		 */
		template<class eType> alwaysInline eType& rtGet(const uint32 index) {
			return (eType&)*(eType*)(__pvt::seq<seqIndex, types...>::rtGet(index));
		}
		alwaysInline void* rtGet(const uint32 index){
			return __pvt::seq<seqIndex, types...>::rtGet(index);
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
		template<class func_t, class ret_t> alwaysInline void exec(func_t pFunc, ret_t& pReturn){
			__pvt::seq<seqIndex, types...>::template exec<func_t, ret_t>(pFunc, &pReturn);
		}
		template<class func_t> alwaysInline void exec(func_t pFunc){
			__pvt::seq<seqIndex, types...>::template exec<func_t, __pvt::NoRet_t>(pFunc, nullptr );
		}




		//pReturn can be omitted to ignore the return value or call void func_tions
		//FIXME return the value directly
		/**
		 * @brief Calls a member func_tion using the array elements as arguments
		 * @param pObject The object to call the func_tion on
		 * @param pFunc The member func_tion to call
		 * @param pReturn A variable where to store the return value
		 */
		template<class obj_t, class func_t, class ret_t> alwaysInline void exec(obj_t& pObject, func_t pFunc, ret_t& pReturn){
			__pvt::seq<seqIndex, types...>::template execObj<obj_t, func_t, ret_t>(pObject, pFunc, &pReturn);
		}
		template<class obj_t, class func_t> alwaysInline void exec(obj_t& pObject, func_t pFunc){
			__pvt::seq<seqIndex, types...>::template execObj<obj_t, func_t, __pvt::NoRet_t>(pObject, pFunc, nullptr );
		}
	};
	#undef seqIndex








	/**
	 * @brief Handy HdCtArray alias
	 */
	template<class... types> struct L : HdCtArray<types...>{
		alwaysInline L() : HdCtArray<types...>(){}
		alwaysInline L(const types&... vals) : HdCtArray<types...>(vals...){}
	};
}
