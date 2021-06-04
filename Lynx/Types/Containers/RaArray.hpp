#pragma once
#define LNX_H_RAARRAY
#include "Lynx/Math/Algebra/Algebra.hpp"
#include "Lynx/Types/Containers/ContainerBase.hpp"
#include "Lynx/Types/Pointer.hpp"
//TODO add shrink function to reorder the elements and use less memory possible
//TODO add self check on unusable elements




/*
.                         RAARRAY
.                   Random Access Array
.
-----------------------------------------------------------------------------------------
.
.	    count: 8
.	    free:  6
.
.	    data  | [0] [1] [2] [3] [4] [5] [6] [7] [8]
.      ---------------------------------------------
.       free  |  y   n   n   y   y   y   y   y   y
.       value |  ?  -53 957  ?   ?   ?   ?   ?   ?
.       next  |  3  -1  -1   5   6   4   ?   ?   ?
.
.                ^ tail: 0               ^ head: 6
.                  next: 3                 next: input
.
-----------------------------------------------------------------------------------------
.
.    Each element has a 'next' index
.    New elements are saved in tail
.    Free indices point to the first index that was freed after them,
.      in order to form a list that is internally linked and bounded by the array
.    Head is the last index that was freed
*/








namespace lnx {
	template<class tType, class tIndex> struct RaArray;

	namespace __pvt{
		template<class tType, class tIndex, bool construct> struct raCtor_t{};
		template<class tType, class tIndex> struct raCtor_t<tType, tIndex, false>{
			protected:
			alwaysInline void initRange(const tIndex& vFrom, const tIndex& vTo) const noexcept {}
		};
		template<class tType, class tIndex> struct raCtor_t<tType, tIndex, true>{
			using arrt = lnx::RaArray<tType, tIndex>;
			protected:
			// inline void initRange(const tIndex vFrom, const tIndex vTo) const {
			// 	type* elm = ((lnx::RaArray<type, tIndex>*)this)->begin();
			// 	for(tIndex i = vFrom; i <= vTo; ++i) {
			// 		new(elm + i) type();
			// 	}
			// }
			//FIXME ADD ADD FUNCTION
			inline void initRange(const tIndex& vFrom, const tIndex& vTo) {
				for(tIndex i = vFrom; i < vTo; ++i) {
					/*if(((arrt*)this)->isValid(i))*/ new(&((arrt*)this)[i]) tType();
				}
			}
			inline void init(const tIndex& vIndex){
				/*if(((arrt*)this)->isValid(vIndex))*/ new(&((arrt*)this)[vIndex]) tType();
			}
		};


		template<class tType, class tIndex, bool destroy> struct raDtor_t{};
		template<class tType, class tIndex> struct raDtor_t<tType, tIndex, false>{
			protected:
			alwaysInline void specializedDestroy() const noexcept {}
		};
		template<class tType, class tIndex> struct raDtor_t<tType, tIndex, true>{
			using arrt = lnx::RaArray<tType, tIndex>;
			protected:
			inline void specializedDestroy() {
				int i = 0;
				for(auto elm : *(arrt*)this) {
					if(((arrt*)this)->isValid(i++)) elm.~tType(); //TODO USE ITERATORS AND NOT INDICES
				}
			}
		};
	}








	/**
	 * @brief A dynamic array with non contiguous elements.
	 *		New elements are written over previously removed ones, or concatenated if there are none.
	 *		The .isValid() function can be used to check if an element is valid or has been removed
	 * @tparam tType Type of the elements
	 * @tparam tIndex Type of the index. The type of any index or count relative to this object depend on this
	 */
	template<class tType, class tIndex = uint32> struct RaArray:
	private __pvt::raCtor_t<tType, tIndex, !std::is_base_of_v<ignoreCopy, tType> && !std::is_trivially_constructible_v<tType>>,
	private __pvt::raDtor_t<tType, tIndex, !std::is_base_of_v<ignoreDtor, tType> && !std::is_trivially_destructible_v <tType>> {
		static_assert(!std::is_void_v<tType>, "RaArray declared as array of void");
		static_assert(
			has_int_conversion_operator_v<tIndex> || std::is_integral_v<tIndex> || std::is_enum_v<tIndex>,
			"tIndex template parameter must be convertible to an integer or have integral or enum type"
		);
		static_assert(std::is_trivial_v<tIndex>, "tIndex template parameter must be a trivial type");


		genInitCheck;


		struct Elm{
			tType value;	//Value of this element
			tIndex next;	//Index of the next free element
		};


		struct Iterator{
			Elm* addr;	//Address of the element pointed by the iterator

			alwaysInline Iterator operator++(int) noexcept { return {   addr++ }; }
			alwaysInline Iterator operator++(   ) noexcept { return { ++addr   }; }
			alwaysInline Iterator operator--(int) noexcept { return {   addr-- }; }
			alwaysInline Iterator operator--(   ) noexcept { return { --addr   }; }

			alwaysInline void operator+=(const uint64 vVal) noexcept { addr += vVal; }
			alwaysInline void operator-=(const uint64 vVal) noexcept { addr += vVal; }
			alwaysInline void operator+=(const int64  vVal) noexcept { addr += vVal; }
			alwaysInline void operator-=(const int64  vVal) noexcept { addr += vVal; }

			alwaysInline Iterator operator+(const uint64 vVal) const noexcept { return { addr + vVal }; }
			alwaysInline Iterator operator-(const uint64 vVal) const noexcept { return { addr - vVal }; }


			alwaysInline tType& operator[](const uint64 vIndex) const { return addr[vIndex].value; }
			alwaysInline tType& operator* (                   ) const { return addr->value; }
			alwaysInline tType* operator->(                   ) const noexcept { return (tType*)addr; }

			alwaysInline bool operator==(tType* vPtr)    const noexcept { return vPtr == (tType*)addr; }
			alwaysInline bool operator!=(tType* vPtr)    const noexcept { return vPtr != (tType*)addr; }
			alwaysInline bool operator==(Iterator vPtr) const { return vPtr.addr == addr; }
			alwaysInline bool operator!=(Iterator vPtr) const { return vPtr.addr != addr; }
		};


	private:
		ram::ptr<Elm> data;

		tIndex tail;	//First free element
		tIndex head;	//Last free element
		tIndex count_;	//Total number of elements
		tIndex free_;	//Number of free elements




		template<class tCType, class tCIndex> inline auto& copyRaArray(const RaArray<tCType, tCIndex>& pArr) {
			static_assert(std::is_convertible_v<tCType, tType> && std::is_convertible_v<tCIndex, tIndex>, "Source array is not compatible");
			isInit(pArr); if(this == &pArr) return *this;

			data.reallocArr(pArr.count(), false);
			tail   = static_cast<tIndex>(pArr.tail);    head  = static_cast<tIndex>(pArr.head);
			count_ = static_cast<tIndex>(pArr.count()); free_ = static_cast<tIndex>(pArr.freeCount());

			//FIXME USE PLAIN COPY FOR TRIVIALLY COPIABLE TYPES
			//TODO BLINDLY COPY FREED ELEMENTS TOGETHER WITH THE INDEX IF THE VALUE IS SMALLER THAN A CERTAIN CONFIGURABLE VALUE
			for(tCIndex i = 0; i < pArr.count(); ++i){
				if(pArr.isValid(i)) new(&(data[static_cast<tIndex>(i)].value)) tType(static_cast<tType >(pArr.data[i].value));
				;                         data[static_cast<tIndex>(i)].next =        static_cast<tIndex>(pArr.data[i].next);
			}
			return *this;
		}




		template<class tCType, class tCIndex> inline auto& copyContainerBase(const ContainerBase<tCType, tCIndex>& pCont){
			static_assert(std::is_convertible_v<tCType, tType> && std::is_convertible_v<tCIndex, tIndex>, "Source array is not compatible");
			isInit(pCont);

			data.reallocArr(pCont.count(), false);
			tail = head = (tIndex)-1;
			count_ = pCont.count(); free_ = 0;

			for(tCIndex i = 0; i < pCont.count(); ++i){
				new(&(data[static_cast<tIndex>(i)].value)) tType(static_cast<tType>(pCont[i]));
				;     data[static_cast<tIndex>(i)].next = (tIndex)-1;
			}
			return *this;
		}




	public:








		// Constructors -------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Creates an array without allocating memory to it
		 *     The memory will be allocated when calling the add function or assigning it an allocated array
		 * Complexity: O(1)
		 */
		inline RaArray() : data(nullptr),
			tail{ (tIndex)-1 }, head{ (tIndex)-1 }, count_{ 0 }, free_{ 0 } {
		}


		/**
		 * @brief Creates an array of size 0 and preallocates the memory for vCount elements
		 *     Preallocated elements are not constructed.
		 * Complexity: O(1)
		 * @param vCount The number of elements to preallocate
		 */
		inline RaArray(const tIndex vCount) :
			checkInitList(dbg::checkParam(vCount < 0, "vCount", "Count must be a positive value"))
			data(sizeof(Elm) * vCount),
			tail{ (tIndex)-1 }, head{ (tIndex)-1 }, count_{ 0 }, free_{ 0 } {
		}




		/**
		 * @brief Initializes the array by copy constructing each element from an std::initializer_list
		 *     Trivially constructible types are not constructed
		 * Complexity: O(n)
		 * @param vElms Initializer list to copy elements from
		 */
		inline RaArray(const std::initializer_list<tType> vElms) :
			// RaArray(vElms.size()) {
			data(sizeof(Elm) * vElms.size()),
			tail{ (tIndex)-1 }, head{ (tIndex)-1 },
			count_{ static_cast<tIndex>(vElms.size()) }, free_{ 0 } {

			// for(const type& elm : vElms) add(elm);
			for(tIndex i = 0; i < vElms.size(); ++i){
				new(&(data[i].value)) tType(*(vElms.begin() + i));
				;     data[i].next = (tIndex)-1;
			}
		}




		/**
		 * @brief Initializes the array by copy constructing each element from a lnx::ContainerBase subclass
		 * Complexity: O(n)
		 * @param pCont The container object to copy elements from
		 *     Its tType and tIndex muse be convertible to this object's tType and tIndex
		 */
		template<class tCType, class tCIndex> inline RaArray(const ContainerBase<tCType, tCIndex>& pCont) {
			copyContainerBase(pCont);
		}


		/**
		 * @brief Initializes the array by copy constructing each element from a RaArray of compatible type
		 *     Removed elements are preserved but not constructed
		 *     Trivially constructible types are not constructed individually but plainly copied as binary data
		 * Complexity: O(n)
		 * @param pArr The RaArray to copy elements from.
		 *     Its tType and tIndex muse be convertible to this object's tType and tIndex
		 */
		template<class tCType, class tCIndex> inline RaArray(const RaArray<tCType, tCIndex>& pArr) {
			copyRaArray(pArr);
		}




		/**
		 * @brief Copy constructor. Initializes the array by copy constructing each element from a RaArray of the same type
		 *     Removed elements are preserved but not constructed
		 *     Trivially constructible types are not constructed individually but plainly copied as binary data
		 * Complexity: O(n)
		 * @param pArr The RaArray to copy elements from
		 */
		inline RaArray(const RaArray<tType, tIndex>& pArr) {
			copyRaArray(pArr);
		}




		/**
		 * @brief Move constructor. Moves pArr to this object. pArr will become invalid
		 *     No element is constructed nor destroyed
		 * Complexity: O(1)
		 * @param pArr The array to move
		 */
		inline RaArray(RaArray<tType, tIndex>&& pArr) : checkInitList(isInit(pArr))
			data{ pArr.data },
			tail{ pArr.tail }, head{ pArr.head }, count_{ pArr.count_ }, free_{ pArr.free_ } {
			pArr.count_ = 0;	//Prevent the destructor from destroying the elements
			//! Data is not destroyed, as the pointer keeps track of how many owners it has
		}




		/**
		 * @brief Destroys each element of the array and frees the used memory
		 *     Trivially destructible types are not destroyed
		 * Complexity:
		 *     O(1) [trivially destructible types]
		 *     O(n) [non trivially destructible types]
		 */
		~RaArray() {
			checkInit();
			if(!empty()) this->specializedDestroy();
			data.realloc(0);
		}








		// Add, remove --------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Adds a new element to the end of the array and initializes it by copying the pData value.
		 * Complexity:
		 *     Best:  O(1)
		 *     Worst: O(n) [memory reallocation required]
		 * @param pData Object to copy construct the new element from
		 * @return Index of the new element
		 */
		tIndex append(const tType& pData) {
			checkInit();
			data.reallocArr(count() + 1);

			new(&(data[count_].value)) tType(pData);	//Initialize new element
			data[count_].next = (tIndex)-1;			//Set the tracker as valid
			return count_++;						//Update the number of elements and return the element index
		}




		/**
		 * @brief Adds a new element in the first free index of the array and initializes it by copying the pData value.
		 * Complexity:
		 *     Best:  O(1)
		 *     Worst: O(n) [no free indices && memory reallocation required]
		 * @param pData Object to copy construct the new element from
 		 * @return Index of the new element
		 */
		tIndex add(const tType& pData) {
			checkInit();
			if(tail == (tIndex)-1) {				//If it has no free elements
				return append(pData);				//Append the new element
			}
			else{
				tIndex prevTail = tail;				//Save tail
				if(tail == head) {					//If it has only one free element
					data[prevTail].next = (tIndex)-1;		//Reset tail and head
					tail = head = (tIndex)-1;				//Reset tracker
				}
				else {								//If it has more than one
					tail = data[prevTail].next;			//Update tail
					data[prevTail].next = (tIndex)-1;		//Update tracker of the old tail element
				}
				free_--;							//Update number of free elements
				new(&(data[prevTail].value)) tType(pData);//Initialize the new element
				return prevTail;						//Return the index of the new element
			}
		}




		/**
		 * @brief Destroys and element and removes it from the array
		 *		The destructor is not called on trivial types or lnx::ignoreDtor subclasses
		 * Complexity: O(1)
		 * @param vIndex Index of the element to remove
		 */
		void remove(const tIndex vIndex) {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Cannot remove element at index %d. It was already deleted", vIndex);

			data[vIndex].value.~tType();					//Destroy the element
			data[vIndex].next = 0;							//Set the index as free
			//!                 ^ 0 is used as a "not -1" value. -1 are valid elements
			if(tail == (tIndex)-1) tail = head = vIndex;	//If it has no free elements, initialize tail and head.
			else {											//If it has free elements
				data[head].next = vIndex;						//Set the new head
				head = vIndex;									//update the last free index
			}
			free_++;										//Update the number of free elements
		}




		/**
		 * @brief Resets the array to its initial state by destroying each element, freeing the memory and resizing it to 0
		 *     Trivially destructible elements are not destroyed
		 * Complexity:
		 *     O(1) [trivially destructible types]
		 *     O(n) [non trivially destructible types]
		 */
		inline void clear() {
			checkInit();
			this->specializedDestroy();
			tail = head = (tIndex)-1;
			count_ = free_ = 0;
			data.reallocArr(0);
		}








		// Assignment ---------------------------------------------------------------------------------------------------------- //







//FIXME CHECK checkInit(); >>>
		/**
		 * @brief Initializes the array by copy constructing each element from a lnx::ContainerBase subclass
		 *    Elements and indices are static_cast casted
		 * Complexity: O(n)
		 * @param pCont The container object to copy elements from.
		 *    It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class tCType, class tCIndex> inline auto& operator=(const ContainerBase<tCType, tCIndex>& pCont){
			this->specializedDestroy();
			return copyContainerBase(pCont);
		}



		/**
		 * @brief Calls the destructor of the elements and copies any element of pCont. Removed indices are preserved but their value is not constructed.
		 *    Trivial types are not destroyed. Elements and indices are static_cast casted
		 * Complexity:
		 *     O(1)     [self assignment]
		 *     O(n)     [trivial types]
		 *     O(n + m) [non trivial types]
		 *     where n = count() and m = pCont.count()
		 * @param pCont The RaArray to copy construct elements from.
		 * @return R-value reference to this object
		 */
		template<class tCType, class tCIndex> alwaysInline auto& operator=(const RaArray<tCType, tCIndex>& pCont) {
			this->specializedDestroy();
			return copyRaArray(pCont);
		}


		/** //FIXME REMOVE. Probably useless. managed in general operator=
		 * @brief Copy assignment. Calls the destructor of the elements and copies any element of pCont. Removed indices are preserved but their value is not constructed.
		 *    Trivial types are not destroyed
		 * Complexity:
		 *     O(1)     [self assignment]
		 *     O(n)     [trivial types]
		 *     O(n + m) [non trivial types]
		 *     where n = count() and m = pCont.count()
		 * @param pCont The RaArray to copy construct elements from.
		 * @return R-value reference to this object
		 */
		alwaysInline auto& operator=(const RaArray<tType, tIndex>& pCont) {
			this->specializedDestroy();
			return copyRaArray(pCont);
		}


//TODO ADD MOVE CONSTRUCTOR FOR GENERIC RAARRAYS
//TODO ADD MOVE CONSTRUCTOR FOR GENERIC CONTAINERS

//TODO ADD MOVE ASSIGNMENT FOR GENERIC RAARRAYS
//TODO ADD MOVE ASSIGNMENT FOR GENERIC CONTAINERS


		/**
		 * @brief Move assignment. Calls the destructor of the old elemets.
		 * Complexity:
		 *     O(1) [trivial types || self assignment]
		 *     O(n) [non trivial types]
		 */
		inline auto& operator=(RaArray<tType, tIndex>&& pCont) {
			isInit(pCont);
			if(this == &pCont) return *this;

			this->destroy();		//Destroy old elements. The pointer doesn't do that
			data = pCont.data;		//Copy array data
			tail   = pCont.tail;    head  = pCont.head;
			count_ = pCont.count(); free_ = pCont.freeCount();
			pCont.count_ = 0;		//Prevent the destructor from destroying the new elements
			//! Data is not destroyed, as the pointer keeps track of how many owners it has
			return *this;
		}








		// Elements state, get element ----------------------------------------------------------------------------------------- //








		/**
		 * @brief Returns the state of an element
		 * @param vIndex Index of the element
		 * @return True if the element is valid (non deleted), false if not
		 */
		alwaysInline bool isValid(const tIndex vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return data[vIndex].next == (tIndex)-1;
		}




		alwaysInline tType& operator[](const tIndex vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Accessing deleted array element");
			return data[vIndex].value;
		}








		// Size ---------------------------------------------------------------------------------------------------------------- //







		//TODO ADD SIZE

		//Returns the number of elements in the map, including the free ones
		alwaysInline tIndex count() const { checkInit(); return count_; }

		//Returns true if the array has no used elements. Equivalent to .usedCount() == 0
		alwaysInline bool empty() const { checkInit(); return !usedCount(); }

		//Returns the number of used elements
		alwaysInline tIndex usedCount() const { checkInit(); return count_ - free_; }

		//Returns the number of free elements
		alwaysInline tIndex freeCount() const { checkInit(); return free_; }

		alwaysInline Iterator begin() const { checkInit(); if(empty()) return { nullptr }; else return { data }; }
		alwaysInline Iterator   end() const { checkInit(); if(empty()) return { nullptr }; else return { data + count_ }; }
	};
}
