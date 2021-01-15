
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
	private:
	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		inline RtArray(           ) : Super(      ) { }
		inline RtArray(iter vCount) : Super(vCount) { }

		inline RtArray(const std::initializer_list<type> vElms) : Super{ vElms } { }


		/**
		 * @brief Initializes the array with a lux::ContainerBase subclass instance by calling the copy constructor on each element
		 * @param pCont The container to copy elements from
		 * @param vConstruct If true, the elements are initialized before calling the copy constructor.
		 *		This is required for arrays containing elements that need to be initialized before calling the copy constructor,
		 *		such as lux::ContainerBase, lux::ram::Alloc or any object that has this type of member.
		 *		This is always false with built-in types
		 */
		template<class cType, class cIter> inline RtArray(const ContainerBase<cType, cIter>& pCont, const bool vConstruct = true) :
			Super(pCont, vConstruct) {
		}


		inline RtArray(const RtArray<type, iter>& pCont) : Super(pCont, true) { }										//copy constructor
		inline RtArray(RtArray<type, iter>&& pCont){ Super::move(pCont); }			//Move constructor
		inline void operator=(const RtArray<type, iter>& pCont){ Super::copy(pCont); /*return*/ }		//copy assignment //FIXME return reference chain
		inline void operator=(RtArray<type, iter>&& pCont){ Super::move(pCont); }	//Move assignment
//BUG move rvalues instead of casting them




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		//Resizes the array without initializing the new elements
		//*   vNewSize | new count of the array
		// //*   Returns  | the new count
		// //TODO totally useless. Just don't return
		// inline iter resize(const iter vNewSize) {
		inline void resize(const iter vNewSize) {
			Super::resize(vNewSize);
			// Super::data.reallocArr(vNewSize, type( ));
			// return Super::data.count( );
		}


		//Resets the array to its initial state, freeing the memory and resizing it to 0
		inline void clear( ){
			checkInit();
			Super::destroy();	//Free old elements
			// Super::data.free();

			//TODO dont call this directly. add construct function
			// this->DynArray::DynArray( );
			//TODO constructor
			Super::data.realloc(0, type(), CellClass::AT_LEAST_CLASS_B);
		}


		//Adds an element to the end of the array
		//*   vElement | the element to add
		//*   Returns  | the index of the element in the array
		inline iter add(const type& vElement) { //BUG NULLPTR NOT CHECKED
			checkInit();
			auto oldCount = Super::count();
			resize(Super::count() + 1);
			operator[](oldCount) = vElement;
			return oldCount;
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //



		//TODO add specific functions for count
		inline uint64 size( ) const { checkInit(); return Super::count( ) * sizeof(type); }


		inline type&  operator[](const iter vIndex) const {
			checkInit();
			lux::dbg::checkCond(Super::count() == 0,                "This function cannot be called on containers with size 0");
			dbg::checkParam(vIndex < 0, "vIndex",         "Index cannot be negative");
			dbg::checkParam(vIndex >= Super::count( ), "vIndex", "Index is out of range");
			return Super::operator[](vIndex);
		}
	};
}


//TODO check if non secure C pointers were used. Like const char* strings