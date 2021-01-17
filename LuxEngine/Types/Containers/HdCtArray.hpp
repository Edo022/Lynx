#define LUX_H_HDCTARRAY
#include "LuxEngine/Types/Integers/Integers.hpp"








namespace lux{
	template<class ...types> struct HdCtArray;
	namespace __pvt{
		//common virtual functions, Seq declaration, unspecialized get_t definition
		#define genGetVFunc inline virtual seq<index, type, types...>* getArr() = 0;
		template<uint32 index, class type, class ...types> struct seq;
		enum __action : uint32{ CONT = (uint32)-1, DESC = 0, GETV = 1 };
		template <__action act, uint32 index, class type, class... types> struct get_t{ };


		//Iterate through elements (get_t specializations)
		template <uint32 index, class type, class... types> struct get_t<CONT, index, type, types...>{
			template <uint32 getIndex> inline auto &getFunc() { return getArr()->lux::__pvt::get_t<(__action)(getIndex == index), index, type, types...>::template getFunc<getIndex>(); }
			genGetVFunc;
		};
		template <uint32 index, class type, class... types> struct get_t<DESC, index, type, types...>{
			template <uint32 getIndex> inline auto &getFunc() { return getArr()->lux::__pvt::seq<index - 1, types...>::template get_t<CONT, index - 1, types...>::template getFunc<getIndex>(); }
			genGetVFunc;
		};
		template <uint32 index, class type, class... types> struct get_t<GETV, index, type, types...>{
			template <uint32 getIndex> inline type &getFunc() { return getArr()->val; }
			genGetVFunc;
		};
		#undef genGetVFunc




		//seq iterator
		template<uint32 index, class type, class ...types> struct seq :
		public get_t<CONT, index, type, types...>,
		public get_t<DESC, index, type, types...>,
		public get_t<GETV, index, type, types...>,
		public seq<index - 1, types...>{
			type val;
			virtual uint64 getOriginalSize() = 0;
			inline virtual seq<index, type, types...>* getArr() override { return this; }
			inline void ctGet(const type& _val, const types&... vals){ val = _val; this->lux::__pvt::seq<index - 1, types...>::ctGet(vals...); }
			inline void* rtGet(const uint32 _index){ return (getOriginalSize() - 1 - index == _index) ? (void*)&val : this->lux::__pvt::seq<index - 1, types...>::rtGet(_index); }
			template<class funcType, class retType, class ...argsTypes> inline void exec(funcType _func, retType* _ret, argsTypes&... _args){
				this->lux::__pvt::seq<index - 1, types...>::template exec<funcType, retType, argsTypes..., type>(_func, _ret, _args..., val);
			}
			template<class objType, class funcType, class retType, class ...argsTypes> inline void execObj(objType& _obj, funcType _func, retType* _ret, argsTypes&... _args){
				this->lux::__pvt::seq<index - 1, types...>::template execObj<objType, funcType, retType, argsTypes...>(_obj, _func, _ret, _args...);
			}
		};



		//Don't use this struct. Just don't
		struct NoRet_t{};
		template<class funcType, class ...argsTypes> struct exec_thr{
			funcType _func;
			lux::HdCtArray<argsTypes...> _args;
		};

		//Execute functions
		template<class funcType, class retType, class ...argsTypes> struct exec_t{
			static void exec(funcType _func, retType* _ret, argsTypes&... _args){ *_ret = _func(_args...); }
		};
		template<class funcType, class ...argsTypes> struct exec_t<funcType, NoRet_t, argsTypes...> {
			static void exec(funcType _func, NoRet_t* _ret, argsTypes&... _args){ _func(_args...); }
		};

		//Execute member functions
		template<class objType, class funcType, class retType, class ...argsTypes> struct execObj_t{
			static void execObj(objType& _obj, funcType _func, retType* _ret, argsTypes&... _args){ *_ret = (_obj.*_func)(_args...); }
		};
		template<class objType, class funcType, class ...argsTypes> struct execObj_t<objType, funcType, NoRet_t, argsTypes...> {
			static void execObj(objType& _obj, funcType _func, NoRet_t* _ret, argsTypes&... _args){ (_obj.*_func)(_args...); }
		};

		//Stop at index 0 (seq specialization)
		template<class type> struct seq<0, type> :
		public get_t<CONT, 0, type>,
		public get_t<DESC, 0, type>,
		public get_t<GETV, 0, type>{
			type val;
			inline virtual seq<0, type>* getArr() override { return this; }
			void ctGet(const type& _val){ val = _val; }
			inline void* rtGet(const uint32 _index){ return (void*)&val; }
			template<class funcType, class retType, class ...argsTypes> inline void exec(funcType _func, retType* _ret, argsTypes&... _args){
				lux::__pvt::exec_t<funcType, retType, argsTypes..., type>::exec(_func, _ret, _args..., val);
			}
			template<class objType, class funcType, class retType, class ...argsTypes> inline void execObj(objType& _obj, funcType _func, retType* _ret, argsTypes&... _args){
				lux::__pvt::execObj_t<objType, funcType, retType, argsTypes..., type>::execObj(_obj, _func, _ret, _args..., val);
			}
		};
	}








	#define seqIndex (sizeof...(types) - 1)
	/**
	 * @brief "Heterogeneous Data Compile Time Array" (lux::__pvt::seq uint32erface).
	 *		An array that constains elements of different types.
	 *		Size and types must be known at compile time.
	 *		e.g.  lux::HdCtArray arr = { 1, false, false };
	 */
	template<class ...types> struct HdCtArray : __pvt::seq<seqIndex, types...>{
		HdCtArray(){}
		HdCtArray(const types&... vals){
			this->lux::__pvt::seq<seqIndex, types...>::ctGet(vals...);
		}


		/**
		 * @brief Returns a reference to an element
		 *		This can only be used with compile time known indices
		 *		Use operator[] to retrieve values in runtime. e.g.
		 * @tparam getIndex The index of the element
		 */
		template<uint32 getIndex> inline auto& get(){
			return this->lux::__pvt::seq<seqIndex, types...>::template get_t<lux::__pvt::CONT, seqIndex, types...>::template getFunc<seqIndex - getIndex>();
		}

		//Returns the address of an element as a void pointer
		inline void* operator[](const uint32 index) { return this->lux::__pvt::seq<seqIndex, types...>::rtGet(index); }
		inline uint32 count() const { return sizeof...(types); }




		/**
		 * @brief Calls a function using the array elements as arguments
		 * @param pFunc: The function to call
		 * @param pReturn: A variable where to store the return value
		 */
		template<class funcType, class retType> inline void exec(funcType pFunc, retType& pReturn){
			this->lux::__pvt::seq<seqIndex, types...>::template exec<funcType, retType>(pFunc, &pReturn);
		}
		template<class funcType>                inline void exec(funcType pFunc                  ){
			this->lux::__pvt::seq<seqIndex, types...>::template exec<funcType, lux::__pvt::NoRet_t>(pFunc, nullptr );
		}




		//pReturn can be omitted to ignore the return value or call void functions
		/**
		 * @brief Calls a member function using the array elements as arguments
		 * @param pObject The object to call the function on
		 * @param pFunc The member function to call
		 * @param pReturn A variable where to store the return value
		 */
		template<class objType, class funcType, class retType> inline void exec(objType& pObject, funcType pFunc, retType& pReturn){
			this->lux::__pvt::seq<seqIndex, types...>::template execObj<objType, funcType, retType>(pObject, pFunc, &pReturn);
		}
		template<class objType, class funcType               > inline void exec(objType& pObject, funcType pFunc                  ){
			this->lux::__pvt::seq<seqIndex, types...>::template execObj<objType, funcType, lux::__pvt::NoRet_t>(pObject, pFunc, nullptr );
		}
	};
	#undef seqIndex


	//HdCtArray alias
	template<class... types> struct L : HdCtArray<types...>{
		L() : HdCtArray(){}
		L(const types&... vals) : HdCtArray(vals){}
	};
}
