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
	template<class type, class iter> struct RaArray;

	namespace __pvt{
		template<class type, class iter, bool construct> struct raCtor_t{};
		template<class type, class iter> struct raCtor_t<type, iter, false>{
			protected:
			alwaysInline void initRange(const iter& vFrom, const iter& vTo) const noexcept {}
		};
		template<class type, class iter> struct raCtor_t<type, iter, true>{
			using arrt = lnx::RaArray<type, iter>;
			protected:
			// inline void initRange(const iter vFrom, const iter vTo) const {
			// 	type* elm = ((lnx::RaArray<type, iter>*)this)->begin();
			// 	for(iter i = vFrom; i <= vTo; ++i) {
			// 		new(elm + i) type();
			// 	}
			// }
			//FIXME ADD ADD FUNCTION
			inline void initRange(const iter& vFrom, const iter& vTo) {
				for(iter i = vFrom; i < vTo; ++i) {
					/*if(((arrt*)this)->isValid(i))*/ new(&((arrt*)this)[i]) type();
				}
			}
			inline void init(const iter& vIndex){
				/*if(((arrt*)this)->isValid(vIndex))*/ new(&((arrt*)this)[vIndex]) type();
			}
		};


		template<class type, class iter, bool destroy> struct raDtor_t{};
		template<class type, class iter> struct raDtor_t<type, iter, false>{
			protected:
			alwaysInline void specializedDestroy() const noexcept {}
		};
		template<class type, class iter> struct raDtor_t<type, iter, true>{
			using arrt = lnx::RaArray<type, iter>;
			protected:
			inline void specializedDestroy() {
				int i = 0;
				for(auto elm : *(arrt*)this) {
					if(((arrt*)this)->isValid(i++)) elm.~type(); //TODO USE ITERATORS AND NOT INDICES
				}
			}
		};
	}








	/**
	 * @brief A dynamic array with non contiguous elements.
	 *		New elements are written over previously removed ones, or concatenated if there are none.
	 *		The .isValid() function can be used to check if an element is valid or has been removed
	 * @tparam type Type of the elements
	 * @tparam iter Type of the index. The type of any index or count relative to this object depend on this
	 */
	template<class type, class iter = uint32> struct RaArray:
	private __pvt::raCtor_t<type, iter, !std::is_base_of_v<ignoreCopy, type> && !std::is_trivially_constructible_v<type>>,
	private __pvt::raDtor_t<type, iter, !std::is_base_of_v<ignoreDtor, type> && !std::is_trivially_destructible_v <type>> {
		static_assert(!std::is_void_v<type>, "lnx::RaArray declared as array of void");
		static_assert(
			has_int_conversion_operator_v<iter> || std::is_integral_v<iter>,
			"iter template parameter must have integral or unscoped enum type"
		);
		static_assert(std::is_trivial_v<iter>, "iter template parameter must be a trivial type");


		genInitCheck;


		struct Elm{
			type value;	//Value of this element
			iter next;	//Index of the next free element
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


			alwaysInline type& operator[](const uint64 vIndex) const { return addr[vIndex].value; }
			alwaysInline type& operator* (                   ) const { return addr->value; }
			alwaysInline type* operator->(                   ) const noexcept { return (type*)addr; }

			alwaysInline bool operator==(type* vPtr)    const noexcept { return vPtr == (type*)addr; }
			alwaysInline bool operator!=(type* vPtr)    const noexcept { return vPtr != (type*)addr; }
			alwaysInline bool operator==(Iterator vPtr) const { return vPtr.addr == addr; }
			alwaysInline bool operator!=(Iterator vPtr) const { return vPtr.addr != addr; }

			// alwaysInline iter index() const noexcept { return (data - addr) / sizeof(Elm); }
		};


	private:
		ram::ptr<Elm> data;

		iter tail;		//First free element
		iter head;		//Last free element
		iter count_;	//Total number of elements
		iter free_;		//Number of free elements




		template<class eType, class iType> inline auto& copyRaArray(const RaArray<eType, iType>& pCont) {
			static_assert(std::is_convertible_v<eType, type> && std::is_convertible_v<iType, iter>, "Source array is not compatible");
			isInit(pCont); if(this == &pCont) return *this;

			data.reallocArr(pCont.count(), false);
			tail   = static_cast<iter>(pCont.tail);    head  = static_cast<iter>(pCont.head);
			count_ = static_cast<iter>(pCont.count()); free_ = static_cast<iter>(pCont.freeCount());

			//TODO BLINDLY COPY FREED ELEMENTS TOGETHER WITH THE INDEX IF THE VALUE IS SMALLER THAN A CERTAIN CONFIGURABLE VALUE
			for(iType i = 0; i < pCont.count(); ++i){
				if(pCont.isValid(i)) new(&(data[static_cast<iter>(i)].value)) type(static_cast<type>(pCont.data[i].value));
				;                          data[static_cast<iter>(i)].next =       static_cast<iter>(pCont.data[i].next);
			}
			return *this;
		}




		template<class eType, class iType> inline auto& copyContainerBase(const ContainerBase<eType, iType>& pCont){
			data.reallocArr(pCont.count(), false);
			tail = head = (iter)-1;
			count_ = pCont.count(); free_ = 0;

			for(iType i = 0; i < pCont.count(); ++i){
				new(&(data[static_cast<iter>(i)].value)) type(static_cast<type>(pCont[i]));
				;     data[static_cast<iter>(i)].next = (iter)-1;
			}
			return *this;
		}




	public:








		// Constructors -------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Creates an array without allocating memory to it.
		 *     The memory will be allocated when calling the add function or assigning it an allocated array
		 * Complexity: O(1)
		 */
		inline RaArray() : data(nullptr),
			tail{ (iter)-1 }, head{ (iter)-1 }, count_{ 0 }, free_{ 0 } {
		}


		/**
		 * @brief Creates an array of size 0 and preallocates the memory for vCount elements
		 * Complexity: O(1)
		 */
		inline RaArray(const iter vCount) :
			checkInitList(dbg::checkParam(vCount < 0, "vCount", "Count must be a positive value"))
			data(sizeof(Elm) * vCount),
			tail{ (iter)-1 }, head{ (iter)-1 }, count_{ 0 }, free_{ 0 } {
		}




		/**
		 * @brief Initializes the array by copy constructing each element from an std::initializer_list
		 * Complexity: O(n)
		 */
		inline RaArray(const std::initializer_list<type> vElms) :
			// RaArray(vElms.size()) {
			data(sizeof(Elm) * vElms.size()),
			tail{ (iter)-1 }, head{ (iter)-1 },
			count_{ static_cast<iter>(vElms.size()) }, free_{ 0 } {

			// for(const type& elm : vElms) add(elm);
			for(iter i = 0; i < vElms.size(); ++i){
				new(&(data[i].value)) type(*(vElms.begin() + i));
				;     data[i].next = (iter)-1;
			}
		}




		/**
		 * @brief Initializes the array by copy constructing each element from a lnx::ContainerBase subclass
		 * Complexity: O(n)
		 * @param pCont The container object to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) {// : RaArray(pCont.count()) {
			static_assert(std::is_convertible_v<eType, type> && std::is_convertible_v<iType, iter>, "Assigned array is not compatible");
			isInit(pCont);
			// //!^ Just in case the engine didn't get segfault'd by the count() call
			// for(iter i = 0; i < pCont.count(); ++i) add(pCont[i]);
			copyContainerBase(pCont);
		}


		/**
		 * @brief Initializes the array by copy constructing each element from a RaArray. Removed elements are preserved but not constructed.
		 * Complexity: O(n)
		 * @param pCont The RaArray to copy elements from.
		 *     It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) {
			copyRaArray(pCont);
		}


		/**
		 * @brief Copy constructor. Initializes the array by copy constructing each element from a RaArray. Removed elements are preserved but not constructed.
		 * Complexity: O(n)
		 * @param pCont Array to copy elements from
		 */
		inline RaArray(const RaArray<type, iter>& pCont) {
			copyRaArray(pCont);
		}


		/**
		 * @brief Move constructor
		 * Complexity: O(1)
		 */
		inline RaArray(RaArray<type, iter>&& pCont) : checkInitList(isInit(pCont))
			data{ pCont.data },
			tail{ pCont.tail }, head{ pCont.head }, count_{ pCont.count_ }, free_{ pCont.free_ } {
			pCont.count_ = 0;	//Prevent the destructor from destroying the elements
			//! Data is not destroyed, as the pointer keeps track of how many owners it has
		}


		/**
		 * @brief Destroys each element of the array and frees the memory. Trivial types are not destroyed
		 * Complexity:
		 *     O(1) [trivial types]
		 *     O(n) [non trivial types]
		 */
		~RaArray() {
			if(!empty()) this->specializedDestroy();
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
		iter append(const type& pData) {
			checkInit();
			data.reallocArr(count() + 1);

			new(&(data[count_].value)) type(pData);	//Initialize new element
			data[count_].next = (iter)-1;			//Set the tracker as valid
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
		iter add(const type& pData) {
			checkInit();
			if(tail == (iter)-1) {				//If it has no free elements
				return append(pData);				//Append the new element
			}
			else{
				iter prevTail = tail;				//Save tail
				if(tail == head) {					//If it has only one free element
					data[prevTail].next = (iter)-1;		//Reset tail and head
					tail = head = (iter)-1;				//Reset tracker
				}
				else {								//If it has more than one
					tail = data[prevTail].next;			//Update tail
					data[prevTail].next = (iter)-1;		//Update tracker of the old tail element
				}
				free_--;							//Update number of free elements
				new(&(data[prevTail].value)) type(pData);//Initialize the new element
				return prevTail;						//Return the index of the new element
			}
		}




		/**
		 * @brief Removes an element and calls its destructor
		 *		The destructor is not called on trivial types or lnx::ignoreDtor subclasses
		 * Complexity: O(1)
		 * @param vIndex Index of the element to remove
		 */
		void remove(const iter vIndex) {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Cannot remove element at index %d. It was already deleted", vIndex);

			data[vIndex].value.~type();					//Destroy the element
			data[vIndex].next = 0;						//Set the index as free
			//!                 ^ 0 is used as a "not -1" value. -1 are valid elements
			if(tail == (iter)-1) tail = head = vIndex;	//If it has no free elements, initialize tail and head.
			else {										//If it has free elements
				data[head].next = vIndex;					//Set the new head
				head = vIndex;								//update the last free index
			}
			free_++;									//Update the number of free elements
		}




		/**
		 * @brief Resets the array to its initial state, freeing all the chunks and resizing it to 0
		 * Complexity:
		 *     O(1) [trivial types]
		 *     O(n) [non trivial types]
		 */
		inline void clear() {
			checkInit();
			this->specializedDestroy();
			tail = head = (iter)-1;
			count_ = free_ = 0;
			data.reallocArr(0); //FIXME FREE
		}








		// Assignment ---------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Initializes the array by copy constructing each element from a lnx::ContainerBase subclass
		 *    Elements and indices are static_cast casted
		 * Complexity: O(n)
		 * @param pCont The container object to copy elements from.
		 *    It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline auto& operator=(const ContainerBase<eType, iType>& pCont){
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
		template<class eType, class iType> alwaysInline auto& operator=(const RaArray<eType, iType>& pCont) {
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
		alwaysInline auto& operator=(const RaArray<type, iter>& pCont) {
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
		inline auto& operator=(RaArray<type, iter>&& pCont) {
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
		alwaysInline bool isValid(const iter vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return data[vIndex].next == (iter)-1;
		}




		alwaysInline type& operator[](const iter vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Accessing deleted array element");
			return data[vIndex].value;
		}








		// Size ---------------------------------------------------------------------------------------------------------------- //







		//TODO ADD SIZE

		//Returns the number of elements in the map, including the free ones
		alwaysInline iter count() const { checkInit(); return count_; }

		//Returns true if the array has no used elements. Equivalent to .usedCount() == 0
		alwaysInline bool empty() const { checkInit(); return !usedCount(); }

		//Returns the number of used elements
		alwaysInline iter usedCount() const { checkInit(); return count_ - free_; }

		//Returns the number of free elements
		alwaysInline iter freeCount() const { checkInit(); return free_; }

		alwaysInline Iterator begin() const { checkInit(); if(empty()) return { nullptr }; else return { data }; }
		alwaysInline Iterator   end() const { checkInit(); if(empty()) return { nullptr }; else return { data + count_ }; }
	};
}
