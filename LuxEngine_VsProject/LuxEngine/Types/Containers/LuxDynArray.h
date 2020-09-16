
#pragma once
#include "LuxArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"

#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"


//TODO a low priority thread reorders the points in the meshes
//TODO If the mesh gets modified, it's sended back to the queue
//TODO if not, the new points are saved and used for rendering in the next frames


//TODO "runtime 3D turbolent flow"


//TODO add .reassign function for containers
//TODO the function simply initializes the container with a pointer without copying the data
//TODO foo.reassign(ptr);
//TODO Array<int> reassign(foo, ptr);
//TODO or maybe it's a bad idea. multiple threads or objects modifying the same data... idk

//TODO dont inherit from array
namespace lux {
	template<class type, class iter = uint32> struct DynArray : public ContainerBase<type, iter> {
	//template<class type, class iter = uint32> struct DynArray : public Array<type, iter> {
		lux_sc_generate_debug_structure_body;
		//lux_sc_generate_debug_structure_body;
		type* data_;	//Elements of the array
		iter size_;		//Size of the array

		iter chunkSize;
		lux_sc_generate_nothing_constructor(DynArray) data_{ data_ }, size_{ size_ }, chunkSize{ chunkSize } { }
		//lux_sc_generate_nothing_constructor(DynArray) chunkSize{ chunkSize } { this->Array<type, iter>::Array( ); }

		////Creates an array with no elements
		////*   vChunkSize | the number of new elements allocated when the array grows
		////*       Larger chunks improve performance but use more memory
		////*       Default at ~500KB (depends on the type)
		//inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : chunkSize(vChunkSize) { this->Array<type, iter>::Array( ); }
		//Creates an array with no elements
		//*   vChunkSize | the number of new elements allocated when the array grows
		//*       Larger chunks improve performance but use more memory
		//*       Default at ~500KB (depends on the type)
		inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : data_{ nullptr }, size_{ 0 }, chunkSize{ vChunkSize }  { }
		//inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : Array<type,iter>::Array()  { chunkSize =vChunkSize ; }
		//inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : chunkSize{ vChunkSize }, size_{ 0 }, data_{ nullptr } { }



		////Resizes the array without initializing the new elements
		////*   vNewSize | new size of the array
		////*   Returns  | the new size
		//inline iter __vectorcall resize(const iter vNewSize) {
		//	lux_sc_F;
		//	Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize));
		//	return this->size_ = vNewSize;
		//}
		//Resizes the array without initializing the new elements
		//*   vNewSize | new size of the array
		//*   Returns  | the new size
		//TODO totally useless. Just don't return
		inline iter __vectorcall resize(const iter vNewSize) {
			lux_sc_F;
			////ram::realloc(data_, vNewSize);
			//this->data_ = (type*)realloc(this->data_, vNewSize);
			//return this->size_ = vNewSize;
			////Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize));
			////return this->size_ = vNewSize;
			////return vNewSize;


			//if(vNewSize < 0) return -1;
			//else
			//if(vNewSize == 0) {
			//	free(this->data_);//TODO dont free with global memory pool
			//	this->data_ = nullptr;
			//	//__lp_data = (type*)realloc(__lp_data, sizeof(type));
			//	return this->size_ = 0;
			//}

			type* __lp_data_r = (type*)realloc(data_, sizeof(type) * vNewSize);
			if(__lp_data_r != nullptr)data_ = __lp_data_r;
			return size_ = vNewSize;

		}

		void clear( ){
			free(data_);//TODO dont free with global memory pool
			data_ = nullptr;
			//__lp_data = (type*)realloc(__lp_data, sizeof(type));
			size_ = 0;
		}




		////Resizes the array and initializes the new elements with a value
		////Use resize(<newSize>) for better performance when initialization is not needed
		////*   vNewSize   | the new size of the array
		////*   vInitValue | the value to use to initialize the new elements
		////*   Returns    | the new size
		//inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
		//	lux_sc_F;
		//	//TODO not secure
		//	Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize), vInitValue);
		//	return this->size_ = vNewSize;
		//}
		//Resizes the array and initializes the new elements with a value
		//Use resize(<newSize>) for better performance when initialization is not needed
		//*   vNewSize   | the new size of the array
		//*   vInitValue | the value to use to initialize the new elements
		//*   Returns    | the new size
		//TODO totally useless. Just don't return
		//inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
		//	lux_sc_F;
		//	//TODO not secure
		//	Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize), vInitValue);
		//	return this->size_ = vNewSize;
		//}





		////Adds an element to the end of the array
		////*   vElement | the element to add
		////*   Returns  | the index of the element in the array
		//inline iter __vectorcall add(const type& vElement) {
		//	lux_sc_F;
		//	this->resize(this->size_ + 1);
		//	this->data_[this->size_ - 1] = vElement;
		//	return this->size_ - 1;
		//}
		//Adds an element to the end of the array
		//*   vElement | the element to add
		//*   Returns  | the index of the element in the array
		inline iter __vectorcall add(const type& vElement) {
			lux_sc_F;
			resize(size_ + 1);
			data_[size_ - 1] = vElement;
			return size_ - 1;
		}




		////Returns a reference to the last element of the array (easy way to array[array.size() - 1])
		//inline type& __vectorcall last( ) { lux_sc_F; return (*this)[this->size_ - 1]; }
		////Returns the size of the array
		//inline iter __vectorcall size( ) const override { lux_sc_F; return this->size_; }
		////Returns true if the array is empty, false if not
		//inline bool __vectorcall empty( ) const override { lux_sc_F; return this->size_ == 0; }

		//inline type* __vectorcall begin( ) const override { lux_sc_F; return &this->data_[0]; }
		//inline type* __vectorcall end( ) const override { lux_sc_F; return &this->data_[this->size_ - 1]; }



		//Returns the size of the array
		inline iter __vectorcall size( )	const override { lux_sc_F; return size_; }
		//Returns true if the array is empty, false if not
		inline bool __vectorcall empty( )	const override { lux_sc_F; return !size_; }

		inline type& __vectorcall operator[](const iter vIndex) const { lux_sc_F; return data_[vIndex]; }
		inline type* __vectorcall begin( )	const override { lux_sc_F; return &data_[0]; }
		inline type* __vectorcall end( )	const override { lux_sc_F; return &data_[size_ - 1]; }

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