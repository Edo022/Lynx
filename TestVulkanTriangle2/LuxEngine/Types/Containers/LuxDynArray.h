
#pragma once
#include "LuxArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"



namespace lux {
	template<class type, class iter = uint32> struct DynArray : public Array<type, iter> {
		iter chunkSize;

		//Creates an array with no elements
		//*   vChunkSize: the number of new elements allocated when the array grows
		//*       Larger chunks improve performance but use more memory
		//*       Default at ~500KB (depends on the type)
		inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : chunkSize{ vChunkSize } { Array<type, iter>::Array(); }



		//Resizes the array without initializing the new elements
		//*   vNewSize: the new size of the array
		//*   Returns the new size
		inline iter __vectorcall resize(const iter vNewSize) {
			Array<type, iter>::resize((vNewSize == 0) ? 0 : step(vNewSize, chunkSize));
			return this->__lp_size = vNewSize;
		}

		//Resizes the array and initializes the new elements with a value
		//Use resize(<newSize>) for better performance when initialization is not needed
		//*   vNewSize: the new size of the array
		//*   vInitValue: the value to use to initialize the new elements
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
			Array<type, iter>::resize((vNewSize == 0) ? 0 : step(vNewSize, chunkSize), vInitValue);
			return this->__lp_size = vNewSize;
		}


		//Adds an element to the end of the array
		//*   vElement: the element to add
		//*   Returns the index of the element in the array
		inline iter __vectorcall add(const type& vElement) {
			return this->resize(this->__lp_size + 1, vElement);
		}

		//Returns a reference to the last element of the array (easy way to array[array.size() - 1])
		inline type& __vectorcall last() { return (*this)[this->__lp_size - 1]; }
		//Returns the size of the array
		inline iter __vectorcall size() const override { return this->__lp_size; }
		//Returns true if the array is empty, false if not
		inline bool __vectorcall empty() const override { return this->__lp_size == 0; }

		inline type* __vectorcall begin() const override { return &this->__lp_data[0]; }
		inline type* __vectorcall end() const override { return &this->__lp_data[this->__lp_size - 1]; }
	};
}