
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



//IMPORTANT
//CELLS CANNOT BE UNALLOCATED
//assigning nullptr will only set the address to nullptr
//the cell itself always contains at least 1 byte

//This is to avoit the need to check for nullptr everywhere




namespace lux {
	//TODO move nullptr 1 initialization to memory
	//TODO manage reallocation with chunks larger than the maximum cell size
	//TODO remove size. It's already in the pointer
	template<class type, class iter = uint32> struct DynArray : public ContainerBase<type, iter> {
		lux_sc_generate_debug_structure_body_func_only;
		ram::ptr<type> data_;	//Elements of the array
		iter size_;				//Size of the array
		//TODO divide static and dynamic pointers.
		//TODO static pointers  can allocate and use memory but no be increased or changed
		//TODO or just make it const


		luxDebug(



		);











		lux_sc_generate_nothing_constructor(DynArray) data_{ data_ }, size_{ size_ } { }
		//! [#] Structure is uninitialized            | >>> NOT CHECKED <<<
		//OK



		//TODO add Minimum CellClass values
		//TODO USE CLASS B
		inline DynArray( ) : size_{ 0 } , data_{ ram::alloc(0) } { }
		//inline DynArray( ) : size_{ 0 } , data_{ nullptr } { }
		// [#] No init required
		//OK


		//inline DynArray( ) : size_{ 0 } , data_{ ram::alloc(0, CellClass::CLASS_B) } { }

		//template<class cIter> inline DynArray(const ContainerBase<type, cIter>& pContainer) : DynArray( ) {
		template<class cIter> inline DynArray(const ContainerBase<type, cIter>& pContainer) : size_{ pContainer.size( ) }, data_{ ram::alloc(pContainer.size( )) } {
			param_error_2(sizeof(cIter) > sizeof(iter), pContainer, "The iterator of a container must be larger than the one of the container used to initialize it");
			isInit(pContainer);
			ram::cpy(pContainer.begin( ), data_, pContainer.bytes( ));
		}
		// [#] No init required
		// [#] Iterator is too small       | k | print error
		// [#] pContainer is uninitialized | k | cprint error
		//OK


		//Resizes the array without initializing the new elements
		//*   vNewSize | new size of the array
		//*   Returns  | the new size
		//TODO totally useless. Just don't return
		inline iter __vectorcall resize(const iter vNewSize) {
			checkInit;


			if(vNewSize < 0) {
				Failure printf("Error in function %s, line %d:\n", __FUNCTION__, __LINE__);
				Failure printf("Invalid value passed to \"%s\" parameter of function \"%s\".\n", vNewSize, __FUNCTION__);
				Failure printf("h");
				throw std::exception("UwU");
			}


			param_error_2(vNewSize < 0, vNewSize, "The size of a container cannot be negative");

			//if(vNewSize){
				//type* __lp_data_r = (type*)realloc(data_, sizeof(type) * vNewSize);
				//if(__lp_data_r != nullptr)data_ = __lp_data_r;
				ram::dRealloc(data_, vNewSize);
				//ram::realloc(data_, vNewSize);
				//return size_ = vNewSize;
				size_ = vNewSize;
			//}
			//else clear( );
			return data_.size( );
		}
		// [#] uninitialized structure   | k | print error
		// [#] vNewSize is negative      | k | print error
		// [#] vNewSize is 0             | k | the dRealloc->alloc function will take care of this


		void clear( ){
			checkInit;
			ram::free(data_);
			this->DynArray::DynArray( );
		}
		// [#] uninitialized structure   | k | print error
		//OK


		//Adds an element to the end of the array
		//*   vElement | the element to add
		//*   Returns  | the index of the element in the array
		inline iter __vectorcall add(const type& vElement) {
			checkInit;
			//luxDebug(if(vElement))
			resize(size_ + 1);
			//*data_.end( ) = vElement;
			//data_[data_.size( ) - 1] = vElement;
			//TODO REMOVE
			////TODO "initializing std array of const char pointers"

			//TODO THIS WORKS
			//type& elm = data_.last( );
			//elm = vElement;
			//TODO BUT THIS DOES NOT
			data_.last( ) = vElement;


			//return size_ - 1;
			return data_.size( ) - 1;
		}
		// [#] uninitialized structure   | k | print error
		// [#] uninitialized vElement    | k | references cannot be uninitialized. and the object will only be copied
		//OK



		//TODO FIX
		inline iter		__vectorcall size( )	const override { checkInit; return size_; }
		// [#] uninitialized structure   | k | print error
		//OK

		//inline iter		__vectorcall size( )	const override { lux_sc_F; return data_.size( ); }
		//TODO FIX
		//TODO DEBUG RESIZE FUNCTIONS


		inline uint64	__vectorcall bytes( )	const override { checkInit; return size_ * sizeof(type); }
		// [#] uninitialized structure   | k | print error
		//OK

		inline bool		__vectorcall empty( )	const override { checkInit; return data_.size( ) == 0;				}
		// [#] uninitialized structure   | k | print error
		//OK

		inline type* __vectorcall begin( )	const override { checkInit; return data_.begin( ); }
		//inline type*	__vectorcall begin( )	const override { lux_sc_F; return data_;			}
		//inline type*	__vectorcall begin( )	const override { lux_sc_F; return &data_[0];			}
		// [#] uninitialized structure   | k | print error
		//OK


		inline type* __vectorcall end( )		const override { checkInit; return data_.end( ); }
		//inline type*	__vectorcall end( )		const override { lux_sc_F; return &data_[size_ - 1];	}
		// [#] uninitialized structure   | k | print error
		//OK



		inline type&	__vectorcall operator[](const iter vIndex) const {
			checkInit;  checkSize;
			param_error_2(vIndex < 0,		 vIndex, "Index cannot be negative");
			param_error_2(vIndex >= size( ), vIndex, "Index is out of range");
			return data_[vIndex];
		}
		// [#] uninitialized structure   | k | print error
		// [#] Index is negative         | k | print error
		// [#] Index is out of range     | k | print error
		// [#] size is 0                 | k | print error
		//OK
	};
}



















































//#pragma once
////#include "LuxArray.h"
//#include "LuxEngine/macros.h"
//#include "LuxEngine/Math/Algebra/Algebra.h"
//#include "LuxEngine/Types/Containers/LuxContainer.h"
//#include "LuxEngine/Types/Nothing.h"
//#include "LuxEngine/Types/Nothing_sc_p.h"
//#include "LuxEngine/Memory/Ram/Memory.h"
//
//
//
//
//
//
//
//
///*
//											LUX CONTAINERS
//
//
//    lux::                       │  StaticArray    ¦  Array         ¦  String          ¦  DynArray       ¦  Map
//────────────────────────────────│─────────────────¦────────────────¦──────────────────¦─────────────────¦───────────────────
//    size                        │  compile time   ¦  run time      ¦  run time        ¦  run time       ¦  run time
//    allocation                  │  malloc         ¦  memory pool   ¦  memory pool     ¦  memory pool    ¦  memory pool
//    data arrangement            │  contiguous     ¦  contiguous    ¦  contiguous      ¦  contiguous     ¦  random
//    memory usage                │  1*             ¦  1*            ¦  1*              ¦  1*             ¦  2**
//                                │                 ¦                ¦                  ¦                 ¦
//                                │                 ¦                ¦                  ¦                 ¦
//                                │                 ¦                ¦                  ¦                 ¦
//    add function                │  -              ¦  -             ¦  +, +=           ¦  .add           ¦  .add, .append
//    resize function             │  -              ¦  .resize, =    ¦  .resize, =      ¦  .resize, =     ¦  -
//                                │                 ¦                ¦                  ¦                 ¦
//                                │                 ¦                ¦                  ¦                 ¦
//    with n = current size, m = new size           ¦                ¦                  ¦                 ¦
//    .add    best  case          │  -              ¦  -             ¦  O(1)            ¦  O(1)           ¦  O(2)
//    .add    worst case          │  -              ¦  -             ¦  O(n + |m-n|)    ¦  O(n+1)         ¦  O(n+2)
//    .resize best  case          │  -              ¦  O(1)          ¦  O(1)            ¦  O(1)           ¦  -
//    .resize worst case          │  -              ¦  O(n + |m-n|)  ¦  O(n + |m-n|)    ¦  O(n + |m-n|)   ¦  -
//
//
//
//	*		sizeof(type) * n
//	**		(sizeof(type) + sizeof(iter)) * n
//
//
//
//*/
//
//
//
//
//
//
//
//
//
//
//
////TODO dont inherit from array
//namespace lux {
//	//template<class type, class iter = uint32> struct DynArray : public Array<type, iter> {
//	template<class type, class iter = uint32> struct DynArray : public ContainerBase<type, iter> {
//		lux_sc_generate_debug_structure_body;
//		type* data_;	//Elements of the array
//		iter size_;		//Size of the array
//		//type* data_{ nullptr };	//Elements of the array
//		//iter size_{ 0 };		//Size of the array
//		iter chunkSize;
//
//
//		//lux_sc_generate_debug_structure_body;
//		//lux_sc_generate_nothing_constructor(DynArray) chunkSize{ chunkSize } { this->Array<type, iter>::Array( ); }
//		lux_sc_generate_nothing_constructor(DynArray) data_{ data_ }, size_{ size_ }, chunkSize{ chunkSize } { }
//
//
//		//Creates an array with no elements
//		//*   vChunkSize | the number of new elements allocated when the array grows
//		//*       Larger chunks improve performance but use more memory
//		//*       Default at ~500KB (depends on the type)
//		inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : chunkSize{ vChunkSize }, size_{ 0 }, data_{ nullptr } { }
//
//
//
//		//Resizes the array without initializing the new elements
//		//*   vNewSize | new size of the array
//		//*   Returns  | the new size
//		//TODO totally useless. Just don't return
//		inline iter __vectorcall resize(const iter vNewSize) {
//			lux_sc_F;
//			//ram::realloc(data_, vNewSize);
//			data_ = (type*)realloc(data_, vNewSize);
//			size_ = vNewSize;
//			//Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize));
//			//return this->size_ = vNewSize;
//			return -1;
//		}
//
//		////Resizes the array and initializes the new elements with a value
//		////Use resize(<newSize>) for better performance when initialization is not needed
//		////*   vNewSize   | the new size of the array
//		////*   vInitValue | the value to use to initialize the new elements
//		////*   Returns    | the new size
//		////TODO totally useless. Just don't return
//		//inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
//		//	lux_sc_F;
//		//	//TODO not secure
//		//	Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize), vInitValue);
//		//	return this->size_ = vNewSize;
//		//}
//
//
//		//Adds an element to the end of the array
//		//*   vElement | the element to add
//		//*   Returns  | the index of the element in the array
//		inline iter __vectorcall add(const type& vElement) {
//			lux_sc_F;
//			resize(size_ + 1);
//			data_[size_ - 1] = vElement;
//			return size_ - 1;
//		}
//
//
//		//Returns the size of the array
//		inline iter __vectorcall size( )	const override { lux_sc_F; return size_; }
//		//Returns true if the array is empty, false if not
//		inline bool __vectorcall empty( )	const override { lux_sc_F; return !size_; }
//
//		inline type& __vectorcall operator[](const iter vIndex) const { lux_sc_F; return data_[vIndex]; }
//		inline type* __vectorcall begin( )	const override { lux_sc_F; return &data_[0]; }
//		inline type* __vectorcall end( )	const override { lux_sc_F; return &data_[size_ - 1]; }
//	};
//}