
#pragma once

#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/macros.h"
#include <initializer_list>
#include "vcruntime_string.h"                         // for memcpy
#include <stdlib.h>                                   // for malloc, realloc




//TODO use faster memcpy
//TODO dont use initializer list
//A static array that knows its size. It can be resized, but it doesn't have add or remove methods
namespace lux {
	template <class type, class iter = uint32> class Array : public ContainerBase<type, iter> {
	public:
		type* __lp_data;	//Elements of the array
		iter __lp_size;	//Size of the array
		#define __lp_lux_static_array_init(_size) __lp_size = _size; __lp_data = (type*)malloc(sizeof(type) * __lp_size)




		// Constructors -----------------------------------------------------------------------------------------------------------------------------//




		//Creates an array with no elements
		inline Array( ) :__lp_size(0), __lp_data(nullptr) { }
		//Sets the size of the array and allocates it, without inizializing the elements
		inline Array(const iter vInitSize) { __lp_lux_static_array_init(vInitSize); }


		//Initializes the array using a list of elements, automatically converting it to the right type
		template<class inType> inline Array(const std::initializer_list<inType>& pElements) {
			__lp_lux_static_array_init((iter)pElements.size( ));
			for(uint64 i = 0; i < pElements.end( ) - pElements.begin( ); ++i) __lp_data[i] = (inType) * (pElements.begin( ) + i);
		}
		//Initializes the array using a list of elements of the same type
		inline Array(const std::initializer_list<type>& pElements) {
			__lp_lux_static_array_init(scast<iter>(pElements.size( )));
			memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
		}


		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   pArray: a pointer to the container object
		template<class elmType> inline Array(const ContainerBase<elmType, iter>& pArray) {
			__lp_lux_static_array_init(pArray.end( ) - pArray.begin( ));
			for(uint64 i = 0; i < pArray.end( ) - pArray.begin( ); ++i) __lp_data[i] = (elmType) * (pArray.begin( ) + i);
		}
		#undef __lp_lux_static_array_init




		// Get, set, begin, end ---------------------------------------------------------------------------------------------------------------------//




		inline iter __vectorcall size( ) const override { return __lp_size; }
		inline bool __vectorcall empty( ) const override { return __lp_size == 0; }

		inline type& __vectorcall operator[](const iter vIndex) const { return __lp_data[vIndex]; }
		inline type* __vectorcall begin( ) const override { return __lp_data; }
		inline type* __vectorcall end( ) const override { return &__lp_data[__lp_size - 1]; }




		// Resize -----------------------------------------------------------------------------------------------------------------------------------//




		//Resizes the array without initializing the new elements
		//*   vNewSize: the new size of the array
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize) {
			if(vNewSize < 0) return -1;
			type* __lp_data_r = (type*)realloc(__lp_data, sizeof(type) * vNewSize);
			if(__lp_data_r != nullptr){
				__lp_size = vNewSize;
				__lp_data = __lp_data_r;
			}
			return __lp_size;
		}

		//Resizes the array and initializes the new elements with a value
		//Use resize(<newSize>) for better performance when initialization is not needed
		//*   vNewSize: the new size of the array
		//*   vInitValue: the value to use to initialize the new elements
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
			//TODO not secure
			if(vNewSize < 0) return -1;
			iter oldSize = __lp_size;
			resize(vNewSize);
			//TODO intrinsic function copy for 16,32,64,128,256,512 bits
			if(vNewSize > oldSize) for(iter i = oldSize; i < vNewSize; ++i) __lp_data[i] = vInitValue;
			return vNewSize;
		}
	};
}
#undef __lp_lux_static_array_init