
#pragma once
#define LUX_H_RTARRAY
#include "LuxEngine/Types/Containers/CtArray.hpp"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"

#include "LuxEngine/Memory/Ram/Memory.hpp"
#include "LuxEngine/Types/Containers/ContainerBase.hpp"

#include "LuxEngine/Core/ConsoleOutput.hpp"
#include <initializer_list>
#include <cstring>

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
	private:
		genInitCheck;
		ram::ptr<type> data_;	//Elements of the array
	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		luxDebug(bool checkNeg(iter n) { luxCheckParam(n < 0, n, "Size cannot be negative"); return true; })
		inline RtArray( ) : data_{ ram::AllocBck<type>(0, CellClass::AT_LEAST_CLASS_B) } { }
		inline RtArray(iter vCount) : constructExec(checkNeg, vCount) data_{ ram::allocArr<type>(sizeof(type), vCount, CellClass::AT_LEAST_CLASS_B) } { }

		//Initializes the array using a container object of a compatible type
		//*   pContainer | The container object to copy elements from
		//*       The pContainer iterator must be of equal or smaller type than the one of the object you are initializing
		template<class cIter> inline RtArray(const ContainerBase<type, cIter>& pContainer) : data_{ ram::allocArr(sizeof(type), pContainer.count( ), CellClass::AT_LEAST_CLASS_B) } {
			luxCheckParam(sizeof(cIter) > sizeof(iter), pContainer, "The iterator of a container must be larger than the one of the container used to initialize it");
			checkInitParam(pContainer);
			ram::cpy(pContainer.begin( ), data_, pContainer.size( ));
		}

		//TODO remove
		//Initializes the array using a list of elements of the same type
		inline RtArray(const std::initializer_list<type>& pElements) : data_{ ram::allocArr(sizeof(type), pElements.size( )) } {
			//TODO ^ C strings get destroyed when the function returns
			//luxCheckParam(pElements.size( ) > count_, pElements, "%d-elements CtArray initialized with %d-elements container.\nA compile time array cannot be initialized with larger containers", count_, pElements.size( ));

			memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
		}


		inline ~RtArray(){
			// data_.~ptr();
		}





		// Add, remove --------------------------------------------------------------------------------------------------------- //




		//Resizes the array without initializing the new elements
		//*   vNewSize | new count of the array
		//*   Returns  | the new count
		//TODO totally useless. Just don't return
		inline iter resize(const iter vNewSize) {
			checkInit; luxCheckParam(vNewSize < 0, vNewSize, "The size of a container cannot be negative");
			ram::reallocArr<type>(data_, sizeof(type), vNewSize, type( ));
			return data_.count( );
		}


		//Resets the array to its initial state, freeing the memory and resizing it to 0
		inline void clear( ){
			checkInit;
			ram::free(data_);

			//TODO dont call this directly. add construct function
			// this->DynArray::DynArray( );
			//TODO constructor
			data_ = ram::AllocBck<type>(0, CellClass::AT_LEAST_CLASS_B);
		}


		//Adds an element to the end of the array
		//*   vElement | the element to add
		//*   Returns  | the index of the element in the array
		inline iter add(const type& vElement) {
			checkInit;
			resize(data_.count() + 1);
			data_.last( ) = vElement;
			return data_.count( ) - 1;
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline iter	  count( )	const override { checkInit; return data_.count( );			}
		inline uint64 size( )	const override { checkInit; return count( ) * sizeof(type);	}
		inline bool	  empty( )	const override { checkInit; return !count( );				}
		inline type*  begin( )	const override { checkInit; return data_.begin( );			}
		inline type*  end( )	const override { checkInit; return data_.end( );			}

		inline type&  operator[](const iter vIndex) const {
			checkInit;  checkCount; luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
			return data_[vIndex];
		}
	};
}


//TODO check if non secure C pointers were used. Like const char* strings