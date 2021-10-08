#pragma once
////#define LNX_H_RAARRAY
#include "Lynx/Math/Algebra/Algebra.hpp"
#include "Lynx/Types/Containers/ContainerBase.hpp"
#include "Lynx/Types/Pointer.hpp"
//TODO add shrink function to reorder the elements and use less memory possible


//TODO add addUnique function


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
.    New elements are saved in tail. When this happens, tail advances to the next free index that was pointed by the old tail
.    Free indices point to the first index that was freed after them,
.      in order to form a list that is internally linked and bounded by the array
.    Head is the last index that was freed
*/








namespace lnx {
	/**
	 * @brief A dynamic array with non contiguous elements.
	 *		New elements are written over previously removed ones, or concatenated if there are none.
	 *		The .isValid() function can be used to check if an element is valid or has been removed
	 * @tparam tType Type of the elements
	 *     This type cannot be void
	 * @tparam tIdxt Type of the indices
	 *     The type of any index or count provided by and used on this object and the additional space used by elements indices all depend on this parameter
	 *     tIdxt must be a non const integral type
	 */
	template<class tType, std::integral tIdxt = uint32> struct RaArray{
		genInitCheck;
		static_assert(!std::is_void_v     <tType>, "RaArray declared as array of void");
		static_assert(!std::is_reference_v<tType>, "RaArray declared as array of references");
		static_assert(!std::is_const_v    <tType>, "RaArray declared as array of const values. The elements of a dynamic array must be assignable");
		static_assert(std::is_integral_v  <tIdxt>, "tIdxt template parameter must be an integer type");
		static_assert(!std::is_const_v    <tIdxt>, "tIdxt cannot be const");



		struct Elm{
			tType value;	//Value of this element
			tIdxt next;		//Index of the next free element
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

		tIdxt tail;		//First free element
		tIdxt head;		//Last free element
		tIdxt count_;	//Total number of elements
		tIdxt free_;	//Number of free elements




		alwaysInline void specDestroy() const noexcept
		requires( std::is_trivially_destructible_v<tType>) {}

		template<class tAType, class tAIdxt> alwaysInline void specCopyArr(const RaArray<tAType, tAIdxt>& pArr)
		requires(std::is_same_v<tIdxt, tAIdxt> && sizeof(tType) == sizeof(tAType) && std::is_trivially_copy_constructible_v<tAType>){
			memcpy(data, pArr.data, pArr.data.size());
		}

		template<class tDst, class tSrc> static alwaysInline void specIdxCnv(tDst& pDst, const tSrc vSrc)
		requires(std::is_same_v<tSrc, tDst>){
			pDst = (tDst)vSrc;
		}




		inline void specDestroy()
		requires(!std::is_trivially_destructible_v<tType>) {
			for(tIdxt i = 0; i < count(); ++i){
				if(isValid(i)) data[(uint64)i].value.~tType();
			}
		}

		/**
		 * @brief Specialized function used to copy data from a RaArray
		 * Complexity: O(n)
		 *     Where n = pArr.count()
		 */
		template<class tAType, class tAIdxt> inline void specCopyArr(const RaArray<tAType, tAIdxt>& pArr)
		requires(!std::is_same_v<tIdxt, tAIdxt> || sizeof(tType) != sizeof(tAType) || !std::is_trivially_copy_constructible_v<tAType>){
			for(tAIdxt i = 0; i < pArr.count(); ++i){
				if(pArr.isValid(i)) new(&(data[(uint64)i].value)) tType(scast<tType>(pArr.data[i].value));
				specIdxCnv(data[(uint64)i].next, pArr.data[i].next);
			}
		}

		/**
		 * @brief (tIdxt)-1 is used as special value, so the actual value depends on the index type.
		 *     This function correctly converts the -1 taking into account the type of the indices
		 */
		template<class tDst, class tSrc> static alwaysInline void specIdxCnv(tDst& pDst, const tSrc vSrc)
		requires(!std::is_same_v<tSrc, tDst>) {
			if(vSrc == (tSrc)-1) pDst = (tDst)-1;
			else pDst = (tDst)vSrc;
		}




		/**
		 * @brief Copies data from another RaArray
		 * Complexity: O(n)
		 *     Where n = pArr.count()
		 * @param pArr Array to copy the elements from
		 * @return Rvalue reference to this object
		 */
		template<class tAType, class tAIdxt> inline auto& copyRaArray(const RaArray<tAType, tAIdxt>& pArr) {
			data.reallocArr((uint64)pArr.count(), false);
			specIdxCnv(tail, pArr.tail);  specIdxCnv(head, pArr.head);
			count_ = (tIdxt)pArr.count(); free_ = (tIdxt)pArr.freeCount();

			specCopyArr<tAType>(pArr);
			return *this;
		}




		/**
		 * @brief Copies data from a ContainerBase
		 * Complexity: O(n)
		 *     Where n = pCont.count()
		 * @param pCont Container to copy the elements from
		 * @return Rvalue reference to this object
		 */
		template<class tCType, class tCIdxt> inline auto& copyContainerBase(const ContainerBase<tCType, tCIdxt>& pCont){
			data.reallocArr((uint64)pCont.count(), false);
			tail = head = (tIdxt)-1;
			count_ = (tIdxt)pCont.count(); free_ = 0;

			for(tCIdxt i = 0; i < pCont.count(); ++i){
				new(&(data[(uint64)i].value)) tType(scast<tType>(pCont[i]));
				;     data[(uint64)i].next = (tIdxt)-1;
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
		inline RaArray() : data(nullptr) {
			tail = head = (tIdxt)-1;
			count_ = free_ = 0;
		}


		/**
		 * @brief Creates an array of size 0 and preallocates the memory for vCount elements
		 *     Preallocated elements are not constructed
		 * Complexity: O(n)
		 * @param vCount The number of elements to preallocate
		 */
		inline RaArray(const tIdxt vCount) :
			checkInitList(dbg::checkParam(vCount < 0, "vCount", "Count must be a positive value"))
			data(sizeof(Elm) * (uint64)vCount) {
			tail = 0; head = (uint64)vCount - 1;
			count_ = vCount; free_ = 0;
			for(tIdxt i = 0; i < vCount; ++i) data[i].next = (uint64)i + 1;
		}




		/**
		 * @brief Initializes the array by copy constructing each element from an std::initializer_list
		 *     Trivially constructible types are not constructed
		 * Complexity: O(n)
		 * @param vElms Initializer list to copy elements from
		 */
		inline RaArray(const std::initializer_list<tType> vElms) :
			data(sizeof(Elm) * (uint64)vElms.size()) {
			tail = (tIdxt)-1; head = (tIdxt)-1;
			count_ = (tIdxt)vElms.size(); free_ = 0;

			// for(const type& elm : vElms) add(elm);
			for(tIdxt i = 0; i < vElms.size(); ++i){
				new(&(data[i].value)) tType(*(vElms.begin() + i));
				;     data[i].next = (tIdxt)-1;
			}
		}




		/**
		 * @brief ContainerBase version of copy constructor. Initializes the array by copy constructing each element from a lnx::ContainerBase subclass
		 * Complexity: O(n)
		 * @param pCont The container object to copy elements from
		 */
		template<class tCType, std::integral tCIdxt> inline RaArray(const ContainerBase<tCType, tCIdxt>& pCont) {
			isInit(pCont);
			copyContainerBase(pCont);
		}


		/**
		 * @brief Templated version of copy constructor. Initializes the array by copy constructing each element from a RaArray of compatible type
		 *     Removed elements are preserved but not constructed
		 *     Trivially destructible types are not destroyed
		 *     Trivially copy constructible types are not constructed individually but plainly copied as binary data
		 * Complexity: O(n)
		 * @param pArr The RaArray to copy elements from
		 */
		template<class tAType, std::integral tAIdxt> inline RaArray(const RaArray<tAType, tAIdxt>& pArr) {
			isInit(pArr);
			copyRaArray(pArr);
		}




		/**
		 * @brief Copy constructor. Initializes the array by copy constructing each element from a RaArray of the same type
		 *     Removed elements are preserved but not constructed
		 *     Trivially destructible types are not destroyed
		 *     Trivially copy constructible types are not constructed individually but plainly copied as binary data
		 * Complexity: O(n)
		 * @param pArr The RaArray to copy elements from
		 */
		inline RaArray(const RaArray<tType, tIdxt>& pArr) {
			isInit(pArr);
			copyRaArray(pArr);
		}




		/**
		 * @brief Move constructor. Moves pArr to this object. pArr will become invalid
		 *     No element is constructed nor destroyed
		 * Complexity: O(1)
		 * @param pArr The array to move
		 */
		inline RaArray(RaArray<tType, tIdxt>&& pArr) : checkInitList(isInit(pArr))
			data{ pArr.data },
			tail{ pArr.tail }, head{ pArr.head }, count_{ pArr.count_ }, free_{ pArr.free_ } {
			pArr.count_ = 0;	//Prevent the destructor from destroying the elements
			// //! Data is not destroyed, as the pointer keeps track of how many owners it has
			pArr.data = nullptr;
		}




		/**
		 * @brief Destructor. Destroys each element of the array and frees the used memory
		 *     Trivially destructible types are not destroyed
		 * Complexity:
		 *     O(1) [trivially destructible types]
		 *     O(n) [non trivially destructible types]
		 */
		~RaArray() {
			checkInit();
			if(data){
				if(!empty()) specDestroy();
				data.realloc(0);
			}
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
		tIdxt append(const tType& pData) {
			checkInit();
			data.reallocArr((uint64)count() + 1);

			new(&(data[count_].value)) tType(pData);	//Initialize new element
			data[count_].next = (tIdxt)-1;				//Set the tracker as valid
			return count_++;							//Update the number of elements and return the element index
		}




		/**
		 * @brief Adds a new element in the first free index of the array and initializes it by copying the pData value.
		 * Complexity:
		 *     Best:  O(1)
		 *     Worst: O(n) [no free indices && memory reallocation required]
		 * @param pData Object to copy construct the new element from
 		 * @return Index of the new element
		 */
		tIdxt add(const tType& pData) {
			checkInit();
			if(tail == (tIdxt)-1) {			//If it has no free elements
				return append(pData);						//Append the new element
			}
			else{
				tIdxt prevTail = tail;					//Save tail
				if(tail == head) {						//If it has only one free element
					data[prevTail].next = (tIdxt)-1;		//Reset tail and head
					tail = head = (tIdxt)-1;				//Reset tracker
				}
				else {									//If it has more than one
					tail = data[prevTail].next;				//Update tail
					data[prevTail].next = (tIdxt)-1;		//Update tracker of the old tail element
				}
				free_--;								//Update number of free elements
				new(&(data[prevTail].value)) tType(pData);//Initialize the new element
				return prevTail;						//Return the index of the new element
			}
		}




		/**
		 * @brief Destroys an element and removes it from the array
		 * Complexity: O(1)
		 * @param vIndex Index of the element to remove
		 */
		void remove(const tIdxt vIndex) {
			checkInit();
			dbg::checkIndex(vIndex, 0, (uint64)count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Cannot remove element at index %d. It was already deleted", vIndex);

			data[vIndex].value.~tType();					//Destroy the element
			data[vIndex].next = 0;							//Set the index as free
			//!                 ^ 0 is used as a "not -1" value. -1 are valid elements
			if(tail == (tIdxt)-1) tail = head = vIndex;	//If it has no free elements, initialize tail and head.
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
			specDestroy();
			tail = head = (tIdxt)-1;
			count_ = free_ = 0;
			data.reallocArr(0);
		}








		// Assignment ---------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief ContainerBase version of copy assignment. Destroys each element and copies the elements of pCont into this array
		 *     Trivially destructible types are not destroyed
		 * Complexity:
		 *     O(n)     [trivially destructible types]
		 *     O(n + m) [non trivially destructible types]
		 *     where n = this->count() and m = pCont.count()
		 * @param pCont The container to copy construct elements from
		 * @return L-value reference to this object
		 */
		template<class tCType, std::integral tCIdxt> inline auto& operator=(const ContainerBase<tCType, tCIdxt>& pCont){
			checkInit(); isInit(pCont);

			specDestroy();
			return copyContainerBase(pCont);
		}



		/**
		 * @brief Templated version of copy assignment. Destroys each element and copies the elements of pCont into this array
		 *     Removed elements are preserved but not constructed
		 *     Trivially destructible types are not destroyed
 		 *     Trivially copy constructible types are not constructed individually but plainly copied as binary data
		 * Complexity:
		 *     O(1)     [self assignment]
		 *     O(n)     [trivially destructible types]
		 *     O(n + m) [non trivially destructible types]
		 *     where n = this->count() and m = pArr.count()
		 * @param pArr The RaArray to copy construct elements from
		 * @return L-value reference to this object
		 */
		template<class tAType, class tAIdxt> alwaysInline auto& operator=(const RaArray<tAType, tAIdxt>& pArr) {
			checkInit(); isInit(pArr);
			if(this == &pArr) return *this;

			specDestroy();
			return copyRaArray(pArr);
		}


		/**
		 * @brief Copy assignment. Destroys each element and copies the elements of pCont into this array
		 *     Removed elements are preserved but not constructed
		 *     Trivially destructible types are not destroyed
 		 *     Trivially copy constructible types are not constructed individually but plainly copied as binary data
		 * Complexity:
		 *     O(1)     [self assignment]
		 *     O(n)     [trivially destructible types]
		 *     O(n + m) [non trivially destructible types]
		 *     where n = this->count() and m = pArr.count()
		 * @param pArr The RaArray to copy construct elements from
		 * @return L-value reference to this object
		 */
		alwaysInline auto& operator=(const RaArray<tType, tIdxt>& pArr) {
			checkInit(); isInit(pArr);
			if(this == &pArr) return *this;

			specDestroy();
			return copyRaArray(pArr);
		}


		/**
		 * @brief Move assignment. Destroys each of the old elements and moved pArr to this array
		 *     New elements are not constructed
		 *     Trivially destructible elements are not destroyed
		 * Complexity:
		 *     O(1) [trivially destructible types || self assignment]
		 *     O(n) [non trivially destructible types]
		 * @param pArr The array to move
		 * @return L-value reference to this object
		 */
		inline auto& operator=(RaArray<tType, tIdxt>&& pArr) {
			checkInit(); isInit(pArr);
			if(this == &pArr) return *this;

			specDestroy();		//Destroy old elements. The pointer doesn't do that
			data   = pArr.data;		//Copy array data
			tail   = pArr.tail;    head  = pArr.head;
			count_ = pArr.count(); free_ = pArr.freeCount();
			pArr.count_ = 0;		//Prevent the destructor from destroying the new elements
			// //! Data is not destroyed, as the pointer keeps track of how many owners it has
			pArr.data = nullptr;
			return *this;
		}








		// Elements state, get element ----------------------------------------------------------------------------------------- //








		/**
		 * @brief Returns the state of an element
		 * Complexity: O(1)
		 * @param vIndex Index of the element
		 * @return True if the element is valid, false if not (deleted or unallocated)
		 */
		alwaysInline bool isValid(const tIdxt vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return data[(uint64)vIndex].next == (tIdxt)-1;
		}




		/**
		 * @brief Returns an element
		 * Complexity: O(1)
		 * @param vIndex Index of the element
		 * @return L-value reference to the element
		 */

		alwaysInline tType& operator[](const tIdxt vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Accessing deleted array element");
			return data[(uint64)vIndex].value;
		}








		// Size ---------------------------------------------------------------------------------------------------------------- //







		//TODO ADD SIZE

		//Returns the number of elements in the map, including the free ones
		alwaysInline tIdxt count() const { checkInit(); return count_; }

		//Returns the number of free elements
		alwaysInline tIdxt freeCount() const { checkInit(); return free_; }

		//Returns the number of used elements
		alwaysInline tIdxt usedCount() const { checkInit(); return count() - freeCount(); }

		//Returns true if the array has no used elements. Equivalent to .usedCount() == 0
		alwaysInline bool empty() const { checkInit(); return !usedCount(); }

		alwaysInline Iterator begin() const { checkInit(); if(empty()) return { nullptr }; else return { data }; }
		alwaysInline Iterator   end() const { checkInit(); if(empty()) return { nullptr }; else return { data + count_ }; }
	};
}
