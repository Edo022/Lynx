
#pragma once
#define LUX_H_RTARRAY
#include "LuxEngine/Types/Containers/ContainerBase.hpp"
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include <initializer_list>
#include <cstring>


//FIXME use always_inline

//TODO a low priority thread reorders the points in the meshes
//TODO If the mesh gets modified, it's sended back to the queue
//TODO if not, the new points are saved and used for rendering in the next frames

//TODO "runtime 3D turbolent flow"


//TODO add .reassign function for containers
//TODO the function simply initializes the container with a pointer without copying the data
//TODO foo.reassign(ptr);
//TODO Array<int> reassign(foo, ptr);
//TODO or maybe it's a bad idea. multiple threads or objects modifying the same data... idk

//TODO add .move function in containers instead of .reassign
//TODO .moved arrays becomes invalid

//TODO add contructor of string from lux containers of chars
//TODO add additional data in errors
namespace lux {
	//"RunTime Array"
	//A dynamic array that uses the global memory pool
	template<class type, class iter = uint32> struct RtArray : public ContainerBase<type, iter> {
		using Super = ContainerBase<type, iter>;
		genInitCheck;




		// Constructors -------------------------------------------------------------------------------------------------------- //




		alwaysInline RtArray(           ) : Super(      ) {}
		alwaysInline RtArray(iter vCount) : Super(vCount) {}

		alwaysInline RtArray(const std::initializer_list<type> vElms) : Super{ vElms } {}


		/**
		 * @brief Initializes the array with a lux::ContainerBase subclass instance by calling the copy constructor on each element
		 * @param pCont The container to copy elements from
		 * @param vConstruct If true, the elements are initialized before calling the copy constructor.
		 *		This is required for arrays containing elements that need to be initialized before calling the copy constructor,
		 *		such as lux::ContainerBase, lux::ram::Alloc or any object that has this type of member.
		 *		This is always false with built-in types
		 */
		template<class cType, class cIter> alwaysInline RtArray(const ContainerBase<cType, cIter>& pCont) :
			Super(pCont, {}) {
		}


		alwaysInline        RtArray(const RtArray<type, iter>&  pCont) : Super(pCont, {}) {  }	//copy constructor
		alwaysInline        RtArray(      RtArray<type, iter>&& pCont) { Super::move(pCont); }	//Move constructor
		alwaysInline void operator=(const RtArray<type, iter>&  pCont) { Super::copy(pCont); }	//copy assignment //FIXME return reference chain
		alwaysInline void operator=(      RtArray<type, iter>&& pCont) { Super::move(pCont); }	//Move assignment




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		/**
		 * @brief Resizes the array and calls the default constructor on each of the new elements
		 * @param vCount New number of elements
		 * @return Number of elements in the array after being resized
		 */
		alwaysInline void resize(const iter vCount) {
			checkInit();
			Super::resize(vCount);
		}


		/**
		 * @brief Resets the array to its initial state, freeing the memory and resizing it to 0.
		 *		Calls the destructor on each element
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
			return size() - 1;
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		//Returns the number of BYTES occupied by the array elements
		//Use count() to get the number of elements
		alwaysInline uint64 size( ) const { checkInit(); return Super::count( ) * sizeof(type); }


		alwaysInline type&  operator[](const iter vIndex) const {
			checkInit();
			dbg::checkCond(Super::count() == 0, "This function cannot be called on containers with size 0");
			dbg::checkIndex(vIndex, 0, Super::count() - 1, "vIndex");
			return Super::operator[](vIndex);
		}
	};
}


//TODO add specific functions for count
//TODO check if non secure C pointers were used. Like const char* strings