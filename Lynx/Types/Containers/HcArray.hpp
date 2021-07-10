#pragma once
#define LNX_H_HCARRAY
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Types/TypeTraits.hpp"








// seq element helper structure -----------------------------------------------------------------------------------------------------------------------//








namespace lnx{
	template<class ...types> struct HcArray;


	namespace __pvt{
		/**
		 * @brief seq helper struct that manages the values based on their type
		 * @tparam tType The type of the element
		 * @tparam tIndex The index of the element. Used to distinguish the bases when there are multiple with the same type
		 */
		template<class tType, uint32 tIndex> struct seq_val{};




		/**
		 * @brief seq_val specialization for non-reference elements
		 *     Used by HcArray only
		 */
		template<class tType, uint32 tIndex> requires(!std::is_reference_v<tType> && !std::is_array_v<tType>) struct seq_val<tType, tIndex>{
			tType val;
			seq_val() = default;
			seq_val(const tType& pVal) : val(pVal) {};

			void operator=(const tType& pElm){ val = pElm; }
			inline tType& get(){ return val; }
		};




		/**
		 * @brief seq_val specialization for reference elements
		 *     Used by HcArray, indirectly used by fwd
		 *     This struct allows reference elements to use operator= by storing them as pointers
		 */
		template<class tType, uint32 tIndex> requires(std::is_reference_v<tType>) struct seq_val<tType, tIndex>{
			std::remove_reference_t<tType>* val;
			seq_val() = default;
			seq_val(const tType& pVal) : val(&pVal) {};

			void operator=(const tType& pElm){ val = &pElm; } //TODO ADD DETAILED ERROR WHEN CALLING = ON REFERENCE ELEMENTS OF HcArray (NOT FWD)
			inline tType get(){ return static_cast<tType>(*val); } //TODO
			//!    ^ Already reference
		};




		/**
		 * @brief seq_val specialization for array elements
		 *     Used by HcArray only
		 *     This struct allows arrays to be initialized by copying them
		 *     Trivially copy assignable types are memcpy ed, non trivially copy assignable types are copied one by one
		 */
		template<class tType, uint32 tIndex> requires(std::is_array_v<tType>) struct seq_val<tType, tIndex>{
			tType val;
			seq_val() = default;
			seq_val(const tType& pVal) requires(std::is_trivially_copy_assignable_v<decltype(pVal[0])>) {
				memcpy(val, pVal, sizeof(tType));
			};
			seq_val(const tType& pVal) requires(!std::is_trivially_copy_assignable_v<std::remove_reference_t<decltype(pVal[0])>>) {
				using elmt = std::remove_cv_t<std::remove_reference_t<decltype(pVal[0])>>;
				for(uint32 i = 0; i < sizeof(tType) / sizeof(elmt); ++i){
					((elmt*)val)[i] = ((elmt*)pVal)[i];
				}
			};

			void operator=(const tType& pElm){} //FIXME ADD DETAILED ERROR WHEN CALLING = ON ARRAY ELEMENTS
			inline tType& get(){ return val; }
		};








		// Runtime get (rtGet function) helper structures ---------------------------------------------------------------------------------------------//








		/**
		 * @brief Enum defining actions for seq_get_t element iterations
		 */
		enum seq_action : uint32{
			eChck = (uint32)-1,
			eDesc = 0,
			eGetv = 1
		};


		/**
		 * @brief Struct used to iterate through the elemenets of the array
		 * @tparam tCount The number of elements in the array
		 * @tparam tAct A seq_action value that defines what to do in the current iteration
		 * @tparam tIndex The index of the element in the array
		 * @tparam tType The type of the element
		 * @tparam tTypes The types of the next elements. Used to recursively inherit the next type
		 */
		template <uint32 tCount, seq_action tAct, uint32 tIndex, class tType, class... tTypes> struct seq_get_t{};


		//seq forward declaration for getFunc functions
		template<uint32 tCount, uint32 tIndex, class tType, class ...tTypes> struct seq;


		//CHCK specialization: Checks if the required index is the same as the current one. If true, returns the element. If false, runs another iteration
		template <uint32 tCount, uint32 tIndex, class tType, class... tTypes> struct seq_get_t<tCount, eChck, tIndex, tType, tTypes...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return ((seq<tCount, tIndex, tType, tTypes...>*)this)->
				seq_get_t<tCount, (seq_action)(getIndex == tIndex), tIndex, tType, tTypes...>::template getFunc<getIndex>();
			}
		};

		//DESC specialization: Executes another iteration and calls its CHCK
		template <uint32 tCount, uint32 tIndex, class tType, class... tTypes> struct seq_get_t<tCount, eDesc, tIndex, tType, tTypes...>{
			template <uint32 getIndex> alwaysInline auto &getFunc() {
				return ((seq<tCount, tIndex, tType, tTypes...>*)this)->
				seq<tCount, tIndex - 1, tTypes...>::template seq_get_t<tCount, eChck, tIndex - 1, tTypes...>::template getFunc<getIndex>();
			}
		};

		//GETV specialization: Stops iteration and returns the element value
		template <uint32 tCount, uint32 tIndex, class tType, class... tTypes> struct seq_get_t<tCount, eGetv, tIndex, tType, tTypes...>{
			template <uint32 getIndex> alwaysInline tType &getFunc() {
				return ((seq<tCount, tIndex, tType, tTypes...>*)this)->seq_val<tType, tIndex>::get();
			}
		};








		// seq class ----------------------------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief The actual array structure
		 *     Each element inherits from the next element
		 * @tparam tCount The number of elements in the array
		 * @tparam tIndex The index of the element in the array
		 * @tparam tType The type of the element
		 * @tparam tTypes The types of the next elements
		 */
		template<uint32 tCount, uint32 tIndex, class tType = void/*Used in empty arrays*/, class ...tTypes> struct seq :
		public seq_val<tType, tIndex>,
		public seq_get_t<tCount, eChck, tIndex, tType, tTypes...>,
		public seq_get_t<tCount, eDesc, tIndex, tType, tTypes...>,
		public seq_get_t<tCount, eGetv, tIndex, tType, tTypes...>,
		public seq<tCount, tIndex - 1, tTypes...> {
			/**
			 * @brief Default constructor
			 *     Each element is initialized with its default value
			 *              ⎛⎲ tCount   ⎛       ⎞⎞
			 * Complexity: O⎝⎳ i=tIndex ⎝O(e[i])⎠⎠
			 * Where e[i] = default constructor of the i-th type in (tType, tTypes)
			 */
			alwaysInline seq():
				seq<tCount, tIndex - 1, tTypes...>(),
				seq_val<tType, tIndex>() {
			}


			/**
			 * @brief List constructor
			 *     Each element is copy constructed with the corresponding value in pVals
			 *              ⎛⎲ tCount   ⎛       ⎞⎞
			 * Complexity: O⎝⎳ i=tIndex ⎝O(e[i])⎠⎠
			 * Where e[i] = copy constructor of the i-th type in (tType, tTypes)
			 */
			alwaysInline seq(const tType& pVal, const tTypes&... pVals) :
				seq<tCount, tIndex - 1, tTypes...>(pVals...),
				seq_val<tType, tIndex>(pVal) { //FIXME
			}


			/**
			 * @brief Returns the address of an element in runtime
			 * Complexity: O(1)
			 * @param vIndex The index of the element
			 * @return The address of the element as a void* pointer
			 */
			inline void* rtGet(const uint32 _index) {
				return (tCount - 1 - tIndex == _index) ?
					(void*)&(seq_val<tType, tIndex>::get()) :
					seq<tCount, tIndex - 1, tTypes...>::rtGet(_index)
				;
			}


			/**
			 * @brief Executes a standard function using the elements as arguments
			 * Complexity: O(O(tFunc))
			 * @tparam tFunc The type of the function. Automatically deduced
			 * @tparam tArgs The types of the arguments. Automatically deduced
			 * @param vFunc The function to execute
			 * @param pArgs A list of references to the arguments of the preceding elements
			 * @return The return value of the called function
			 */
			template<class tFunc, class ...tArgs> alwaysInline auto exec(tFunc vFunc, const tArgs&... pArgs) {
				return seq<tCount, tIndex - 1, tTypes...>::template exec<tFunc, tArgs..., tType>(
					vFunc, pArgs..., seq_val<tType, tIndex>::get()
				);
			}


			/**
			 * @brief Executes a member function using the elements as arguments
			 * Complexity: O(O(tFunc))
			 * @tparam tObj The type of the object. Automatically deduced
			 * @tparam tFunc The type of the function. Automatically deduced
			 * @tparam tArgs The types of the arguments. Automatically deduced
			 * @param pObj The object to call the function on
			 * @param vFunc The function to execute
			 * @param pArgs A list of references to the arguments of the preceding elements
			 * @return The return value of the called function
		 *               ︵
			 */
			template<class tObj, class tFunc, class ...tArgs> alwaysInline auto execObj(tObj& pObj, tFunc vFunc, const tArgs&... pArgs) {
				return seq<tCount, tIndex - 1, tTypes...>::template execObj<tObj, tFunc, tArgs...>(
					pObj, vFunc, pArgs..., seq_val<tType, tIndex>::get()
				);
			}


			/**
			 * @brief Default copy assignment
			 */
			inline seq& operator=(const seq& pSeq) = default;
		};








		/**
		 * @brief seq specialization used to stop recursion in elements with index 0
		 */
		template<uint32 tCount, class tType> struct seq<tCount, 0, tType> :
		public seq_val<tType, 0>,
		public seq_get_t<tCount, eChck, 0, tType>,
		public seq_get_t<tCount, eDesc, 0, tType>,
		public seq_get_t<tCount, eGetv, 0, tType> {
			/**
			 * @brief Default constructor
			 * Complexity: O(O(e))
			 * Where e = default constructor of tType
			 */
			alwaysInline seq() : seq_val<tType, 0>() { }


			/**
			 * @brief List constructor
			 * Complexity: O(O(e))
			 * Where e = copy constructor of tType
			 * @param pVal The value used to copy construct this element
			 */
			alwaysInline seq(const tType& pVal) :
				seq_val<tType, 0>(pVal) {
			}

			/**
			 * @brief Runtime get. Same as general specialization
			 */
			alwaysInline void* rtGet(const uint32 _index) { return (void*)&(seq_val<tType, 0>::get()); }

			/**
			 * @brief Executes a standard function. Same as general specialization
			 */
			template<class func_t, class ...args_ts> alwaysInline auto exec(func_t _func, const args_ts&... _args) {
				return _func(_args..., seq_val<tType, 0>::get());
			}

			/**
			 * @brief Executes a member function. Same as general specialization
			 */
			template<class obj_t, class func_t, class ...args_ts> alwaysInline auto execObj(obj_t& _obj, func_t _func, const args_ts&... _args) {
				return (_obj.*_func)(_args..., seq_val<tType, 0>::get());
			}

			/**
			 * @brief Default copy assignment
			 */
			inline seq& operator=(const seq& pSeq) = default;
		};








		/**
		 * @brief seq specialization for empty arrays
		 * This struct can only run functions with no arguments
		 */
		template<uint32 index> struct seq<0, index, void> {
			seq() = default;
			seq(const seq& pSeq) = default;

			template<class func_t> alwaysInline auto exec(func_t _func) {
				return _func();
			}
			template<class obj_t, class func_t, class ...args_ts> alwaysInline auto execObj(obj_t& _obj, func_t _func) {
				return (_obj.*_func)();
			}

			inline seq& operator=(const seq& pSeq) = default;
		};
	}








	// User interface -----------------------------------------------------------------------------------------------------------------------------//








	//Forward declaration for friend class
	template<class... tTypes> struct P;

	//Starting index of element iteration
	#define seqIndex (sizeof...(tTypes) - 1)

	namespace __pvt{
		//Special type used by fwd
		struct seq_fwd_ctor{};
	}




	/**
	 * @brief An array that can contain elements of different types
	 *     Size and types must be known at compile time
	 *     The structure provides a default constructor, a copy constructor and a list constructor
	 *     The copy constructor is only called when passing an HdArray with the same exact types
	 *     e.g.
	 *         HcArray arr1{ 1, false };								//int, bool
	 *         HcArray arr2(arr1);  									//int, bool
	 *         HcArray<uint32, bool> arr3(arr1)							//Compilation error
	 * @tparam tTypes The types of the elements. Those types can be automatically deduced from the constructor arguments
	 *     e.g.
	 *         HcArray<int, float> arr1{ 1, 0.5f };						//int, float
	 *         HcArray arr2{ 1, false, "mogu mogu" };					//int, bool, char[9]
	 *         HcArray arr3{ 1, false, (const char*)"mogu mogu" };		//int, bool, const char*
	 */
	template<class... tTypes> struct HcArray :
	private __pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>{
	private:
		template<class... _tTypes> friend struct P;

		/**
		 * @brief List constructor
		 *     Used by lnx::fwd only
		 *     Parameters are taken by value as they are all references
		 */
		alwaysInline HcArray(const __pvt::seq_fwd_ctor, const tTypes... vals) :
			__pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>(vals...){
		}

		/**
		 * @brief Default constructor
		 *     Used by lnx::fwd only
		 * Complexity: O()
		 */
		alwaysInline HcArray(const __pvt::seq_fwd_ctor) :
			__pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>() {
		}

	public:
		HcArray(const HcArray<tTypes...>& pArr) = default;
		alwaysInline HcArray() :
			__pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>() {
		}

		alwaysInline HcArray(const tTypes&... vals) requires(!(sizeof...(tTypes) == 1 &&
		std::is_same_v<std::remove_reference_t<std::remove_cv_t<get_type_at_t<0, tTypes...>>>, HcArray<tTypes...>>)) :
			__pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>(vals...){
		}


		/**
		 * @brief Returns a reference to an element.
		 *		This can only be used with compile time known indices.
		 *		Use rtGet<type>(index) to retrieve values in runtime
		 * @tparam vIndex The index of the element
		 */
		template<uint32 vIndex> alwaysInline auto& get() requires(vIndex < sizeof...(tTypes)) {
			// _dbg(static_assert(vIndex >= 2, "Index is out of range"));
			return __pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>:: template seq_get_t<sizeof...(tTypes), __pvt::eChck, seqIndex, tTypes...>::template getFunc<seqIndex - vIndex>();
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
			return (tCType&)*(tCType*)(__pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>::rtGet(vIndex));
		}
		/**
		 * @brief Returns the element address a a void*
		 * @param index The index of the element
		 */
		alwaysInline void* rtGet(const uint32 vIndex) {
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return __pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>::rtGet(vIndex);
		}

		/**
		 * @brief Returns the number of elements in the array
		 */
		alwaysInline constexpr uint32 count() const { return sizeof...(tTypes); }




		/**
		 * @brief Calls a function using the array elements as arguments
		 * @param pFunc: The function to call
		 * @return The return value of the function call
		 */
		template<class func_t> alwaysInline auto exec(func_t pFunc)
		requires(std::is_function_v<std::remove_pointer_t<func_t>>) {
			return __pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>::template exec<func_t>(pFunc);
		}




		/**
		 * @brief Calls a member function using the array elements as arguments
		 * @param pObject The object to call the function on
		 * @param pFunc The member function to call
		 * @return The return value of the function call
		 */
		template<class obj_t, class func_t> alwaysInline auto exec(obj_t& pObject, func_t pFunc)
		requires(std::is_object_v<obj_t> && std::is_member_function_pointer_v<func_t>) {
			return __pvt::seq<sizeof...(tTypes), seqIndex, tTypes...>::template execObj<obj_t, func_t>(pObject, pFunc);
		}

		inline HcArray& operator=(const HcArray& pArr) = default;
	};
	// template<class... tTypesc> HcArray(tTypesc&&...) -> HcArray<tTypesc...>;
	#undef seqIndex








	/**
	 * @brief Handy HcArray alias
	 */
	// template<class... tTypes> struct L : HcArray<tTypes...>{
	// 	alwaysInline L() : HcArray<tTypes...>() {}
	// 	template<class... tTypesc> alwaysInline L(tTypesc&&... vals) : HcArray<tTypes...>((std::forward<tTypes>(vals))...) {}
	// };
	// template<class... tTypesc> L(tTypesc&&...) -> L<tTypesc...>;
	template<class... tTypes> using L = HcArray<tTypes...>;


	/**
	 * @brief Type used to perfect forward a list of arguments as a single parameter
	 */
	template<class... tTypes> struct P : public HcArray<tTypes...>{
		// alwaysInline P() : HcArray<tTypes...>() {}
		inline P(const P<tTypes...>& pObj) = default;

		template<class... tTypesc> alwaysInline P(tTypesc&&... vals) :
			HcArray<tTypes...>(__pvt::seq_fwd_ctor{}, std::forward<tTypesc>(vals)...) {
		}
		//!Copy and move constructors are shadowed by the list constructor

		inline P& operator=(const P& pObj) = default;
		// template<class tType> void operator=(tType&&) = default;
		// operator HcArray<tTypes...>(){
		// 	HcArray<tTypes...> ret;
		// 	memcpy(&ret, this, sizeof(*this));
		// 	return (HcArray<tTypes...>&)(ret);
		// }
	};
	template<class... tTypesc> P(tTypesc&&... vElms) -> P<tTypesc&&...>;
}
