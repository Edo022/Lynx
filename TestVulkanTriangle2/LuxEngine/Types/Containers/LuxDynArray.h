
#pragma once
#include "LuxArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"




template<class type, class alloc = uint32> struct LuxDynArray : public LuxArray<type, alloc> {
	alloc chunkSize;

	//Creates an array with no elements
	//*   vChunkSize: the number of new elements allocated when the array grows
	//*       Larger chunks improve performance but use more memory
	//*       Default at ~500KB (depends on the type)
	inline LuxDynArray(const alloc vChunkSize = fit(sizeof(type), 500000)) : chunkSize{ vChunkSize } { LuxArray<type, alloc>::LuxArray(); }



	//Resizes the array without initializing the new elements
	//*   vNewSize: the new size of the array
	//*   Returns the new size
	inline alloc __vectorcall resize(const alloc vNewSize) {
		LuxArray<type, alloc>::resize((vNewSize == 0) ? 0 : step(vNewSize, chunkSize));
		return this->__lp_size = vNewSize;
	}

	//Resizes the array and initializes the new elements with a value
	//Use resize(<newSize>) for better performance when initialization is not needed
	//*   vNewSize: the new size of the array
	//*   vInitValue: the value to use to initialize the new elements
	//*   Returns the new size. (alloc)-1 if the size is invalid
	inline alloc __vectorcall resize(const alloc vNewSize, const type& vInitValue) {
		LuxArray<type, alloc>::resize((vNewSize == 0) ? 0 : step(vNewSize, chunkSize), vInitValue);
		return this->__lp_size = vNewSize;
	}


	//Adds an element to the end of the array
	//*   vElement: the element to add
	//*   Returns the index of the element in the array
	inline alloc __vectorcall add(const type& vElement) {
		return this->resize(this->__lp_size + 1, vElement);
	}

	//Returns a reference to the last element of the array (easy way to array[array.size() - 1])
	inline type& last(){ return (*this)[this->__lp_size - 1]; }
	//inline type& __vectorcall operator [] (const alloc vIndex){return (*this).LuxArray::__lp_data[vIndex]; }
	inline alloc __vectorcall size() const override { return this->__lp_size; }
	inline type* __vectorcall end() const override { return &this->__lp_data[this->__lp_size - 1]; }
};