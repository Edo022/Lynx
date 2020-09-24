
#pragma once
#include "LuxArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"

#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"

#include "LuxEngine/Core/ConsoleOutput.h"


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
	template<class type, class iter = uint32> struct DynArray : public ContainerBase<type, iter> {
	private:
		lux_sc_generate_debug_structure_body_func_only;
		ram::ptr<type> data_;	//Elements of the array
	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		lux_sc_generate_nothing_constructor(DynArray) data_{ data_ } { }
		//! [#] Structure is uninitialized            | >>> NOT CHECKED <<<
		inline DynArray( ) : data_{ ram::AllocBck<type>(0, CellClass::AT_LEAST_CLASS_B) } { }


		//Initializes the array using a container object of a compatible type
		//*   pContainer | The container object to copy elements from
		//*       The pContainer iterator must be of equal or smaller type than the one of the object you are initializing
		template<class cIter> inline DynArray(const ContainerBase<type, cIter>& pContainer) : data_{ ram::allocBck(pContainer.size( )) } {
			param_error_2(sizeof(cIter) > sizeof(iter), pContainer, "The iterator of a container must be larger than the one of the container used to initialize it");
			isInit(pContainer);
			ram::cpy(pContainer.begin( ), data_, pContainer.bytes( ));
		}




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		//Resizes the array without initializing the new elements
		//*   vNewSize | new size of the array
		//*   Returns  | the new size
		//TODO totally useless. Just don't return
		inline iter __vectorcall resize(const iter vNewSize) {
			checkInit; param_error_2(vNewSize < 0, vNewSize, "The size of a container cannot be negative");
			ram::reallocDB(data_, vNewSize);
			return data_.size( );
		}


		//Resets the array to its initial state, freeing the memory and resizing it to 0
		inline void __vectorcall clear( ){
			checkInit;
			ram::free(data_);
			this->DynArray::DynArray( );
		}


		//Adds an element to the end of the array
		//*   vElement | the element to add
		//*   Returns  | the index of the element in the array
		inline iter __vectorcall add(const type& vElement) {
			checkInit;
			resize(data_.size() + 1);
			data_.last( ) = vElement;
			return data_.size( ) - 1;
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline iter		__vectorcall size( )	const override { checkInit; return data_.size( );			}
		inline uint64	__vectorcall bytes( )	const override { checkInit; return size( ) * sizeof(type);	}
		inline bool		__vectorcall empty( )	const override { checkInit; return !size( );				}
		inline type*	__vectorcall begin( )	const override { checkInit; return data_.begin( );			}
		inline type*	__vectorcall end( )		const override { checkInit; return data_.end( );			}

		inline type&	__vectorcall operator[](const iter vIndex) const {
			checkInit;  checkSize; param_error_2(vIndex < 0, vIndex, "Index cannot be negative"); param_error_2(vIndex >= size( ), vIndex, "Index is out of range");
			return data_[vIndex];
		}
	};
}


