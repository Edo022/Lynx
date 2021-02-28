#pragma once
#define LUX_H_RTARRAY
#include "LuxEngine/Types/Containers/ContainerBase.hpp"








//TODO a low priority thread reorders the points in the meshes
//TODO If the mesh gets modified, it's sent back to the queue
//TODO if not, the new points are saved and used for rendering in the next frames
//TODO "runtime 3D turbolent flow"

//TODO add contructor of string from lux containers of chars
namespace lux {
	/**
	 * @brief A dynamic array that uses the global memory pool
	 * @tparam type Type of the elements
	 * @tparam iter Type of the index. The type of any index or count relative to this object depend on this
	 */
	template<class type, class iter = uint32> struct RtArray : public ContainerBase<type, iter> {
		using Super = ContainerBase<type, iter>;
		genInitCheck;




		// Constructors -------------------------------------------------------------------------------------------------------- //




		/**
		 * @brief Creates an array without allocating memory to it.
		 *		The memory will be allocated when calling the add or resize funcytions
		 */
		alwaysInline RtArray() : Super() {}


		/**
		 * @brief Creates an array of vCount elements and calls the default constructor on each of them
		 *		The constructor is not called on trivial types or lux::ignoreCopy subclasses
		 */
		alwaysInline RtArray(iter vCount) : Super(vCount) {}


		/**
		 * @brief Creates an array by copying the vElm elements. Each element is copy constructed
		 */
		alwaysInline RtArray(const std::initializer_list<type> vElms) : Super{ vElms } {}


		/**
		 * @brief Initializes the array with a lux::ContainerBase subclass instance by copy constructing on each element
		 *		The constructor is not called on trivial types or lux::ignoreCopy subclasses
		 * @param pCont The container to copy elements from
		 */
		template<class cType, class cIter> alwaysInline RtArray(const ContainerBase<cType, cIter>& pCont) :
			Super(pCont, {}) {
		}




		/**
		 * @brief Copy constructor. Each element is copy constructed.
		 *		The constructor is not called on trivial types or lux::ignoreCopy subclasses
		 */
		alwaysInline RtArray(const RtArray<type, iter>& pCont) : Super(pCont, {}) {  }
		/**
		 * @brief Copy assignment. All the elements in the array are destroyed. New elements are copy constructed.
		 *		The destructor  is not called on trivial types or lux::ignoreDtor subclasses.
		 *		The constructor is not called on trivial types or lux::ignoreCopy subclasses
		 */
		alwaysInline auto& operator=(const RtArray<type, iter>& pCont) { Super::copy(pCont); return *this; }


		//Move constructor
		alwaysInline RtArray(RtArray<type, iter>&& pCont) { Super::move(pCont); }
		//Move assignment
		alwaysInline auto& operator=(RtArray<type, iter>&& pCont) { Super::move(pCont); return *this; }




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		#if !defined(LUX_DEBUG) || defined(__INTELLISENSE__)
			/**
			 * @brief Resizes the array. If the type is not a trivial type or a lux::ignoreCopy subclass, calls the constructor on each of the new elements
			 * @param vCount New number of elements
			 */
			alwaysInline void resize(const iter vCount) {
				checkInit();
				Super::resize(vCount);
			}
		#else //Check for negative count
			void resize(int64 vCount) {
				dbg::checkParam(vCount < 0, "vCount", "Count cannot be negative");
				checkInit();
				Super::resize(vCount);
			}
		#endif


		/**
		 * @brief Resets the array to its initial state by freeing the memory and resizing it to 0.
		 *		If the type is not a trivial type or a lux::ignoreDtor subclass, calls the destructor on each element
		 */
		alwaysInline void clear() {
			checkInit();
			Super::destroy();
			Super::data.realloc(0);
		}


		/**
		 * @brief Adds an element to the end of the array and initializes it with the vElement value by calling its copy constructor
		 * @param vElm The element to add
		 * @return The index of the new element
		 */
		alwaysInline iter add(const type& vElm) {
			checkInit();
			Super::cat1(vElm);
			return Super::count() - 1;
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		/**
		 * @brief Returns the number of BYTES occupied by the array elements.
		 *		Use count() to get the number of elements
		 */
		alwaysInline uint64 size( ) const { checkInit(); return Super::count( ) * sizeof(type); }


		alwaysInline type& operator[](const iter vIndex) const {
			checkInit();
			dbg::checkCond(Super::count() == 0, "This function cannot be called on containers with size 0");
			dbg::checkIndex(vIndex, 0, Super::count() - 1, "vIndex");
			return Super::operator[](vIndex);
		}
	};
}