#define LUX_H_HDCTARRAY
#include "LuxEngine/Types/Integers/Integers.hpp"























namespace lux{
	template<class ...types> struct HdCtArray;


		// Runtime get (operator[]) helper structures -------------------------------------------------------------------------------------------------//








	namespace __pvt{
		enum __action : uint32{ CHCK = (uint32)-1, DESC = 0, GETV = 1 };					//Enum defining actions for get_t element iterations
		template <__action act, uint32 index, class type, class... types> struct get_t{ };	//Unspecialized get_t class. This is used to iterate through the elemenets of the array
		template<uint32 index, class type, class ...types> struct seq;						//seq forward declaration for getArr function
		#define genGetVFunc alwaysInline virtual seq<index, type, types...>* getArr() = 0;		//getArr function shared by all the get_t specializations. Returns the HdCtArray object address

		//CHCK specialization: Checks if the required index is the same as the current one. If true, returns the element. If false, runs another iteration
		template <uint32 index, class type, class... types> struct get_t<CHCK, index, type, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return getArr()->lux::__pvt::get_t<(__action)(getIndex == index), index, type, types...>::template getFunc<getIndex>();
			}
			genGetVFunc;
		};

		//DESC specialization: Executes another iteration and calls its CHCK
		template <uint32 index, class type, class... types> struct get_t<DESC, index, type, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return getArr()->lux::__pvt::seq<index - 1, types...>::template get_t<CHCK, index - 1, types...>::template getFunc<getIndex>();
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








		// Execute (exec function) helper structures --------------------------------------------------------------------------------------------------//









		struct NoRet_t{};											//Dummy return type for void functions
		template<class fType, class ...aTypes> struct exec_thr{		//Structure containing the function call informations
			fType _func;												//Function pointer
			lux::HdCtArray<aTypes...> _args;							//Function arguments
		};


		//Executes a non-void non-member function
		template<class fType, class rType, class ...aTypes> struct exec_t{
			static alwaysInline void exec(fType _func, rType* _ret, aTypes&... _args){ *_ret = _func(_args...); }
		};
		//exec_t specialization. Executes a void non-member function //FIXME REMOVE. merge with automatic return value
		template<class fType, class ...aTypes> struct exec_t<fType, NoRet_t, aTypes...> {
			static alwaysInline void exec(fType _func, NoRet_t* _ret, aTypes&... _args){ _func(_args...); }
		};


		//Executes a non-void member function
		template<class oType, class fType, class rType, class ...aTypes> struct execObj_t{
			static alwaysInline void execObj(oType& _obj, fType _func, rType* _ret, aTypes&... _args){ *_ret = (_obj.*_func)(_args...); }
		};
		//execObj_t specialization. Executes a void member function //FIXME REMOVE. merge with automatic return value
		template<class oType, class fType, class ...aTypes> struct execObj_t<oType, fType, NoRet_t, aTypes...> {
			static alwaysInline void execObj(oType& _obj, fType _func, NoRet_t* _ret, aTypes&... _args){ (_obj.*_func)(_args...); }
		};








		// seq class (the actual array structure) -----------------------------------------------------------------------------------------------------//








		template<uint32 index, class type, class ...types> struct seq :
		public get_t<CHCK, index, type, types...>,
		public get_t<DESC, index, type, types...>,
		public get_t<GETV, index, type, types...>,
		public seq<index - 1, types...>{
			type val;
			alwaysInline virtual constexpr uint64 getOriginalSize() = 0;											//Returns the array size. Overridden by HdCtArray
			alwaysInline virtual seq<index, type, types...>* getArr() override { return this; }	//USed by get_t structures

			//List initialization
			alwaysInline void init(const type& _val, const types&... vals){
				val = _val; this->lux::__pvt::seq<index - 1, types...>::init(vals...);
			}

			//Runtime get
			inline void* rtGet(const uint32 _index){
				return (getOriginalSize() - 1 - index == _index) ?
					(void*)&val :
					this->lux::__pvt::seq<index - 1, types...>::rtGet(_index)
				;
			}

			//Executes a standard function
			template<class fType, class rType, class ...aTypes> alwaysInline void exec(fType _func, rType* _ret, aTypes&... _args){
				this->lux::__pvt::seq<index - 1, types...>::template exec<fType, rType, aTypes..., type>(_func, _ret, _args..., val);
			}

			//Executes a member function
			template<class oType, class fType, class rType, class ...aTypes> alwaysInline void execObj(oType& _obj, fType _func, rType* _ret, aTypes&... _args){
				this->lux::__pvt::seq<index - 1, types...>::template execObj<oType, fType, rType, aTypes...>(_obj, _func, _ret, _args...);
			}

			//fType:  Function type
			//rType:  Return type
			//aTypes: Function arguments types
			//oType:  Object type
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
			template<class fType, class rType, class ...aTypes> alwaysInline void exec(fType _func, rType* _ret, aTypes&... _args){
				lux::__pvt::exec_t<fType, rType, aTypes..., type>::exec(_func, _ret, _args..., val);
			}
			template<class oType, class fType, class rType, class ...aTypes> alwaysInline void execObj(oType& _obj, fType _func, rType* _ret, aTypes&... _args){
				lux::__pvt::execObj_t<oType, fType, rType, aTypes..., type>::execObj(_obj, _func, _ret, _args..., val);
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
			this->lux::__pvt::seq<seqIndex, types...>::init(vals...);
		}
		alwaysInline virtual uint64 getOriginalSize(){ return count(); } //FIXME move to template


		/**
		 * @brief Returns a reference to an element.
		 *		This can only be used with compile time known indices.
		 *		Use rtGet<type>(index) to retrieve values in runtime
		 * @tparam vIndex The index of the element
		 */
		template<uint32 vIndex> alwaysInline auto& get(){
			return this->lux::__pvt::seq<seqIndex, types...>::template get_t<lux::__pvt::CHCK, seqIndex, types...>::template getFunc<seqIndex - vIndex>();
		}

		/**
		 * @brief Runtime version of get. Returns a reference to an element.
		 *		Requires the element type to be explicitly specified.   e.g. arr.get<int>(4); //Returns the 4th element as an int&
		 * @param vIndex The index of the element
		 * @tparam eType The type of the element
		 */
		template<class eType> alwaysInline eType& rtGet(const uint32 index) {
			return (eType&)*(eType*)(this->lux::__pvt::seq<seqIndex, types...>::rtGet(index));
		}

		/**
		 * @brief Returns the number of elements in the array
		 */
		alwaysInline constexpr uint32 count() const { return sizeof...(types); }




		//FIXME return the value directly
		/**
		 * @brief Calls a function using the array elements as arguments
		 * @param pFunc: The function to call
		 * @param pReturn: A variable where to store the return value
		 */
		template<class fType, class rType> alwaysInline void exec(fType pFunc, rType& pReturn){
			this->lux::__pvt::seq<seqIndex, types...>::template exec<fType, rType>(pFunc, &pReturn);
		}
		template<class fType> alwaysInline void exec(fType pFunc){
			this->lux::__pvt::seq<seqIndex, types...>::template exec<fType, lux::__pvt::NoRet_t>(pFunc, nullptr );
		}




		//pReturn can be omitted to ignore the return value or call void functions
		//FIXME return the value directly
		/**
		 * @brief Calls a member function using the array elements as arguments
		 * @param pObject The object to call the function on
		 * @param pFunc The member function to call
		 * @param pReturn A variable where to store the return value
		 */
		template<class oType, class fType, class rType> alwaysInline void exec(oType& pObject, fType pFunc, rType& pReturn){
			this->lux::__pvt::seq<seqIndex, types...>::template execObj<oType, fType, rType>(pObject, pFunc, &pReturn);
		}
		template<class oType, class fType> alwaysInline void exec(oType& pObject, fType pFunc){
			this->lux::__pvt::seq<seqIndex, types...>::template execObj<oType, fType, lux::__pvt::NoRet_t>(pObject, pFunc, nullptr );
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
