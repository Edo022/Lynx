#pragma once
////#define LNX_H_RTARRAY
#include "Lynx/Types/Containers/ContainerBase.hpp"
//TODO a low priority thread reorders the points in the meshes
//TODO If the mesh gets modified, it's sent back to the queue
//TODO if not, the new points are saved and used for rendering in the next frames
//TODO "runtime 3D turbolent flow"

//TODO add contructor of string from lnx containers of chars








namespace lnx {
	/**
	 * @brief A dynamic array that uses the global memory pool
	 * @tparam tType Type of the elements
	 * @tparam tIdxt Type of the index. The type of any index or count relative to this object depend on this
	 */
	template<class tType, class tIdxt = uint32> struct RtArray : public ContainerBase<tType, tIdxt> { //TODO dont allow non integer indices
		//!static asserts in ContainerBase
		using Super = ContainerBase<tType, tIdxt>;
		genInitCheck;








		// Constructors -------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Creates an array without allocating memory to it
		 *     The memory will be allocated when calling the add or resize functions
		 * Complexity: O(1)
		 */
		alwaysInline RtArray() : Super() {}


		/**
		 * @brief Count  constructor
		 *     Creates an array of vCount elements and calls the default constructor on each of them
		 *     The constructor is not called on trivially default constructible types
		 *     O(1)  [Trivially default constructible types]
		 *     O(n)  [Non trivially default constructible types]
		 * @param vCount The number of elements
		 */
		alwaysInline RtArray(tIdxt vCount) : Super(vCount) {}


		/**
		 * @brief List constructor
		 *     Creates an array by copying the vElm elements
		 * Complexity: O(n)
		 * @param vElm an std::initializer_list that contains the elements to initialize the array with
		 *     e.g. RtArray<int> a{0, 1, 2};
		 */
		alwaysInline RtArray(const std::initializer_list<tType> vElms) : Super{ vElms } {}


		/**
		 * @brief Initializes the array with a lnx::ContainerBase subclass object by copy constructing each element
		 *     Trivially copy constructible types are not constructed but copied as binary data
		 * Complexity: O(n)
		 * @param pCont The container to copy elements from
		 */
		template<class cType, class cIter> alwaysInline RtArray(const ContainerBase<cType, cIter>& pCont) :
			Super(pCont, {}) {
		}




		/**
		 * @brief Copy constructor
		 *     Trivially copy constructible types are not constructed but copied as binary data
		 * Complexity: O(n)
		 * @param pCont The array to copy elements from
		 */
		alwaysInline RtArray(const RtArray<tType, tIdxt>& pCont) : Super(pCont, {}) {  }


		/**
		 * @brief Copy assignment
		 *     All the elements in the array are destroyed. New elements are copy constructed
		 *     The destructor  is not called on trivially destructible types
		 *     Trivially copy constructible types are not constructed but copied as binary data
		 * Complexity: O(n+m)
		 *     Where n = this->count() and m = pCont.count()
		 */
		alwaysInline auto& operator=(const RtArray<tType, tIdxt>& pCont) { Super::copy(pCont); return *this; }


		/**
		 * @brief Move constructor
		 * Complexity: O(1)
		 */
		alwaysInline RtArray(RtArray<tType, tIdxt>&& pCont) { Super::move(pCont); }


		/**
		 * @brief Move assignment
		 * Complexity: O(1)
		 */
		alwaysInline auto& operator=(RtArray<tType, tIdxt>&& pCont) { Super::move(pCont); return *this; }


		//! No destructor required
		//! Resources are destroyed from ContainerBase destructor







		// Add, remove --------------------------------------------------------------------------------------------------------- //








		#if !defined(LNX_DBG) || defined(__INTELLISENSE__)
			/**
			 * @brief Resizes the array
			 *     Calls the constructor on each of the new elements
			 *     The constructor is not called on trivially default constructible types
			 * Complexity:
			 *     O(1)  [Trivially default constructible types]
			 *     O(n)  [Non trivially default constructible types]
			 * @param vCount The number of elements
			 */
			alwaysInline void resize(const tIdxt vCount) {
				checkInit();
				Super::resize(vCount);
			}
		#else //Check for negative count //FIXME
			void resize(int64 vCount) {
				dbg::checkParam(vCount < 0, "vCount", "Count cannot be negative");
				checkInit();
				Super::resize(vCount);
			}
			//! If in debug mode, the count takes an int64 and checks for negative values.
			//! an int64 is enough, as the count can't be more than 2^32 anyway
		#endif


		/**
		 * @brief Resets the array to its initial state by freeing the memory and resizing it to 0
		 *     Calls the destructor on each element
		 *     Trivially destructible types are not destroyed
		 * Complexity:
		 *     O(1)  [Trivially destructible types]
		 *     O(n)  [Non trivially destructible types]
		 */
		alwaysInline void clear() {
			checkInit();
			Super::destroy();
			Super::data.realloc(0);
		}


		/**
		 * @brief Adds an element at the end of the array
		 *     The new element is copy constructed from vElm
		 * Complexity:
		 *     Best:  O(1)
		 *     Worst: O(n) [Memory reallocation required]
		 * @param vElm The value of the new element
		 * @return The index of the new element
		 */
		alwaysInline tIdxt add(const tType& vElm) {
			checkInit();
			Super::cat1(vElm);
			return Super::count() - 1;
		}

		/**
		 * @brief Adds an element at the end of the array, only if there are no other elements with the same value
		 *     The new element is copy constructed from vElm
		 * Complexity:
		 *     Best:  O(1)
		 *     Worst: O(n)
		 * @param vElm The value of the new element
		 * @return The index of the new element, or (tIdxt)-1 if the array already contains at least one element with the same value
		 */
		alwaysInline tIdxt addUnique(const tType& vElm){
			if(!Super::data) return add(vElm);
			for(const auto& elm : *this){
				if(elm == vElm) return (tIdxt)-1;
			}
			return add(vElm);
		}








		// Get ----------------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Returns the number of bytes occupied by the array elements.
		 *		Use count() to get the number of elements
		 * Complexity: O(1)
		 */
		alwaysInline uint64 size() const { checkInit(); return Super::count() * sizeof(tType); }


		/**
		 * @brief Returns a reference to an element
		 * Complexity: O(1)
		 * @param vIndex The index of the element
		 * @return A rvalue reference to the vIndex-th element
		 */
		alwaysInline tType& operator[](const tIdxt vIndex) const {
			checkInit();
			dbg::checkCond(Super::count() == 0, "This function cannot be called on containers with size 0");
			dbg::checkIndex(vIndex, 0, Super::count() - 1, "vIndex");
			return Super::operator[](vIndex);
		}
	};
}