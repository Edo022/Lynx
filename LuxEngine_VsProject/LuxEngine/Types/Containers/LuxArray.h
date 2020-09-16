#pragma once
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO use faster memcpy
//TODO dont use initializer list
//A static array that knows its size. It can be resized, but it doesn't have add or remove functions
namespace lux {
	template <class type, class iter = uint32> class Array : public ContainerBase<type, iter> {
	public:
		lux_sc_generate_debug_structure_body;
		type* data_;	//Elements of the array
		iter size_;		//Size of the array
		//type* data_{ nullptr };	//Elements of the array
		//iter size_{ 0 };		//Size of the array

		//Debug only
		luxDebug(void checkSize(const iter vInitSize){
			if(vInitSize == scast<iter>(-1) && vInitSize > 0){
				Failure printf("Error:");
				Failure printf("Something went wrong .-. You were trying to allocate %u bytes of memory", scast<iter>(vInitSize));
				Failure printf("This error will not be reported in release mode. Make sure to fix it");
				system("pause"); exit(-1);
			}
		});



		// Constructors -----------------------------------------------------------------------------------------------------------------------------//


		//TODO check all the parameters
		lux_sc_generate_nothing_constructor(Array) data_{ data_ }, size_{ size_ } { }
		//Creates an array with no elements
		inline Array( ) : size_{ 0 }, data_{ nullptr } { }
		//Sets the size of the array and allocates it, without inizializing the elements
		inline Array(const iter vInitSize) : size_{ vInitSize }, data_{ (type*)malloc(sizeof(type) * vInitSize) } {
			luxDebug(checkSize(vInitSize));
			//__lp_lux_static_array_init((vInitSize >= 0) ? vInitSize : 0);
		}

		//TODO remove
		#define __lp_lux_static_array_init(_size) size_ = _size; data_ = (type*)malloc(sizeof(type) * size_)
		//TODO remove
		//TODO use lux EverythingArray
		//Initializes the array using a list of elements, automatically converting it to the right type
		template<class inType> inline Array(const std::initializer_list<inType>& pElements) {
			__lp_lux_static_array_init((iter)pElements.size( ));
			for(uint64 i = 0; i < pElements.end( ) - pElements.begin( ); ++i) data_[i] = (inType) * (pElements.begin( ) + i);
		}
		//TODO remove
		//Initializes the array using a list of elements of the same type
		inline Array(const std::initializer_list<type>& pElements) {
			__lp_lux_static_array_init(scast<iter>(pElements.size( )));
			memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
		}


		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   pArray: a pointer to the container object
		template<class elmType> inline Array(const ContainerBase<elmType, iter>& pArray) : size_{ pArray.size( ) }, data_{ (type*)malloc(sizeof(type) * pArray.size( )) } {
			//__lp_lux_static_array_init(pArray.end( ) - pArray.begin( ));
			for(uint64 i = 0; i < pArray.end( ) - pArray.begin( ); ++i) data_[i] = (elmType) * (pArray.begin( ) + i);
		}
		#undef __lp_lux_static_array_init




		// Get, set, begin, end ---------------------------------------------------------------------------------------------------------------------//




		inline iter __vectorcall size( )	const override	{ lux_sc_F; return size_; }
		inline bool __vectorcall empty( )	const override	{ lux_sc_F; return !size_; }

		inline type& __vectorcall operator[](const iter vIndex) const { lux_sc_F; return data_[vIndex]; }
		inline type* __vectorcall begin( )	const override	{ lux_sc_F; return data_; }
		inline type* __vectorcall end( )	const override	{ lux_sc_F; return &data_[size_ - 1]; }




		// Resize -----------------------------------------------------------------------------------------------------------------------------------//




		//Resizes the array without initializing the new elements
		//*   vNewSize: the new size of the array
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize) {
			lux_sc_F;
			luxDebug(checkSize(vNewSize));
			//if(vNewSize < 0) return -1;
			//else if(vNewSize == 0) {
			//	free(data_);//TODO dont free with global memory pool
			//	data_ = nullptr;
			//	//__lp_data = (type*)realloc(__lp_data, sizeof(type));
			//	return size_ = 0;
			//}

			type* __lp_data_r = (type*)realloc(data_, sizeof(type) * vNewSize);
			if(__lp_data_r != nullptr)data_ = __lp_data_r;
			return size_ = vNewSize;
		}


		void clear( ){
				free(data_);//TODO dont free with global memory pool
				data_ = nullptr;
				//__lp_data = (type*)realloc(__lp_data, sizeof(type));
				return size_ = 0;
		}

		//TODO remove
		//Resizes the array and initializes the new elements with a value
		//Use resize(<newSize>) for better performance when initialization is not needed
		//*   vNewSize: the new size of the array
		//*   vInitValue: the value to use to initialize the new elements
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
			lux_sc_F;
			luxDebug(checkSize(vNewSize));
			//TODO not secure
			if(vNewSize < 0) return -1;
			iter oldSize = size_;
			resize(vNewSize);
			//TODO use intrinsic function copy with automatic loop unwrap
			if(vNewSize > oldSize) for(iter i = oldSize; i < vNewSize; ++i) data_[i] = vInitValue;
			return vNewSize;
		}
	};
}
#undef __lp_lux_static_array_init


