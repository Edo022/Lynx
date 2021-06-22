#pragma once
#define LNX_H_HCARRAY
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Types/TypeTraits.hpp"







namespace lnx{
		template<class ...types> struct HcArray;

		// Runtime get (rtGet function) helper structures ---------------------------------------------------------------------------------------------//








	namespace __pvt{
		enum __action : uint32{ eChck = (uint32)-1, eDesc = 0, eGetv = 1 };								//Enum defining actions for get_t element iterations
		template <uint32 size, __action act, uint32 index, class tType, class... types> struct get_t{};	//Unspecialized get_t class. This is used to iterate through the elemenets of the array
		template<uint32 size, uint32 index, class tType, class ...types> struct seq;						//seq forward declaration for getArr function

		//CHCK specialization: Checks if the required index is the same as the current one. If true, returns the element. If false, runs another iteration
		template <uint32 size, uint32 index, class tType, class... types> struct get_t<size, eChck, index, tType, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return ((seq<size, index, tType, types...>*)this)->
				get_t<size, (__action)(getIndex == index), index, tType, types...>::template getFunc<getIndex>();
			}
		};

		//DESC specialization: Executes another iteration and calls its CHCK
		template <uint32 size, uint32 index, class tType, class... types> struct get_t<size, eDesc, index, tType, types...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return ((seq<size, index, tType, types...>*)this)->
				seq<size, index - 1, types...>::template get_t<size, eChck, index - 1, types...>::template getFunc<getIndex>();
			}
		};

		//GETV specialization: Stops iteration and returns the element value
		template <uint32 size, uint32 index, class tType, class... types> struct get_t<size, eGetv, index, tType, types...>{
			template <uint32 getIndex> alwaysInline tType &getFunc() {
				return ((seq<size, index, tType, types...>*)this)->val;
			}
		};








		// seq class (the actual array structure) -----------------------------------------------------------------------------------------------------//








		template<uint32 size, uint32 index, class tType, class ...types> struct seq :
		public get_t<size, eChck, index, tType, types...>,
		public get_t<size, eDesc, index, tType, types...>,
		public get_t<size, eGetv, index, tType, types...>,
		public seq<size, index - 1, types...>{
			tType val;

			//List initialization
			// alwaysInline void init(const tType& _val, const types&... vals) {
			// 	val = _val; seq<size, index - 1, types...>::init(vals...);
			// }
			alwaysInline seq(const tType& _val, const types&... vals) : seq<size, index - 1, types...>((std::forward<types>(vals))...), val(std::forward<tType>(_val)) { //FIXME
				val = _val; seq<size, index - 1, types...>::init(vals...);
			}

			//Runtime get
			inline void* rtGet(const uint32 _index) {
				return (size - 1 - index == _index) ?
					(void*)&val :
					seq<size, index - 1, types...>::rtGet(_index)
				;
			}

			//Executes a standard function
			template<class func_t, class ...args_ts> alwaysInline auto exec(func_t _func, args_ts&... _args) {
				return seq<size, index - 1, types...>::template exec<func_t, args_ts..., tType>(_func, _args..., val);
			}

			//Executes a member function
			template<class obj_t, class func_t, class ...args_ts> alwaysInline auto execObj(obj_t& _obj, func_t _func, args_ts&... _args) {
				return seq<size, index - 1, types...>::template execObj<obj_t, func_t, args_ts...>(_obj, _func, _args...);
			}
		};








		//Stop at index 0 (seq specialization)
		template<uint32 size, class tType> struct seq<size, 0, tType> :
		public get_t<size, eChck, 0, tType>,
		public get_t<size, eDesc, 0, tType>,
		public get_t<size, eGetv, 0, tType>{
			tType val;
			// alwaysInline void init(const tType& _val) { val = _val; }
			alwaysInline seq(tType& _val) : val(std::forward<tType>(_val)) { }

			alwaysInline void* rtGet(const uint32 _index) { return (void*)&val; }
			template<class func_t, class ...args_ts> alwaysInline auto exec(func_t _func, args_ts&... _args) {
				// return exec_t<func_t, args_ts..., type>::exec(_func, _args..., val);
				return _func(_args..., val);
			}
			template<class obj_t, class func_t, class ...args_ts> alwaysInline auto execObj(obj_t& _obj, func_t _func, args_ts&... _args) {
				// return execObj_t<obj_t, func_t, args_ts..., type>::execObj(_obj, _func, _args..., val);
				return (_obj.*_func)(_args..., val);
			}
		};
	}








	// User interface -----------------------------------------------------------------------------------------------------------------------------//







	//Starting index of element iteration. I'm too lazy to write this everywhere
	#define seqIndex (sizeof...(tType) - 1)
	/**
	 * @brief "Heterogeneous Data Compile Time Array".
	 *		An array that constains elements of different types.
	 *		Size and types must be known at compile time.
	 *		e.g. --- lnx::HcArray arr = { 1, false, "mogu mogu" }; ---
	 */
	template<class... tType> struct HcArray : __pvt::seq<sizeof...(tType), seqIndex, tType...>{
		alwaysInline HcArray() {}
		// alwaysInline HcArray(tType... vals) {
		// 	__pvt::seq<sizeof...(tType), seqIndex, tType...>::init(vals...);
		// }
		alwaysInline HcArray(tType... vals) requires(std::is_reference_v<get_type_at<0, tType...>>) :
			__pvt::seq<sizeof...(tType), seqIndex, tType...>((std::forward<tType>(vals))...){
		}

		alwaysInline HcArray(tType&... vals) requires(!std::is_reference_v<get_type_at<0, tType...>>) :
			__pvt::seq<sizeof...(tType), seqIndex, tType...>((std::forward<tType>(vals))...){
		}


		/**
		 * @brief Returns a reference to an element.
		 *		This can only be used with compile time known indices.
		 *		Use rtGet<type>(index) to retrieve values in runtime
		 * @tparam vIndex The index of the element
		 */
		template<uint32 vIndex> alwaysInline auto& get() requires(vIndex < sizeof...(tType)) {
			// _dbg(static_assert(vIndex >= 2, "Index is out of range"));
			return __pvt::seq<sizeof...(tType), seqIndex, tType...>::template get_t<sizeof...(tType), __pvt::eChck, seqIndex, tType...>::template getFunc<seqIndex - vIndex>();
		}

		/**
		 * @brief Runtime version of get. Returns a reference to an element.
		 *		Requires the element type to be explicitly specified.
		 *		e.g. --- arr.get<int>(4); -- returns the 4th element as an int&.
		 *		If the type is not specified, the element address is returned as a void*
		 * @param vIndex The index of the element
		 * @tparam tCType The type of the element
		 */
		template<class tCType> alwaysInline tCType& rtGet(const uint32 vIndex) {
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return (tCType&)*(tCType*)(__pvt::seq<sizeof...(tType), seqIndex, tType...>::rtGet(vIndex));
		}
		/**
		 * @brief Returns the element address a a void*
		 * @param index The index of the element
		 */
		alwaysInline void* rtGet(const uint32 vIndex) {
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return __pvt::seq<sizeof...(tType), seqIndex, tType...>::rtGet(vIndex);
		}

		/**
		 * @brief Returns the number of elements in the array
		 */
		alwaysInline constexpr uint32 count() const { return sizeof...(tType); }




		/**
		 * @brief Calls a function using the array elements as arguments
		 * @param pFunc: The function to call
		 * @return The return value of the function call
		 */
		template<class func_t> alwaysInline auto exec(func_t pFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			return __pvt::seq<sizeof...(tType), seqIndex, tType...>::template exec<func_t>(pFunc);
		}




		/**
		 * @brief Calls a member function using the array elements as arguments
		 * @param pObject The object to call the function on
		 * @param pFunc The member function to call
		 * @return The return value of the function call
		 */
		template<class obj_t, class func_t> alwaysInline auto exec(obj_t& pObject, func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			return __pvt::seq<sizeof...(tType), seqIndex, tType...>::template execObj<obj_t, func_t>(pObject, pFunc);
		}
	};
	#undef seqIndex








	/**
	 * @brief Handy HcArray alias
	 */
	template<class... types> struct L : HcArray<types...>{
		alwaysInline L() : HcArray<types...>() {}
		alwaysInline L(types&... vals) : HcArray<types...>((std::forward<types>(vals))...) {}
	};
}
