
#pragma once

#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include <initializer_list>
#include <algorithm>





//A static array that knows its size. It can be resized, but it doesn't have add or remove methods
template <class type>
class LuxArray : public LuxContainer<type> {
public:
	type* __lp_data;	//Elements of the array
	uint64 __lp_size;	//Size of the array
	#define __lp_lux_static_array_init(_size) __lp_size = _size; __lp_data = (type*)malloc(sizeof(type) * __lp_size)




	// Constructors -----------------------------------------------------------------------------------------------------------------------------//




	//Creates an array with no elements
	inline __vectorcall LuxArray() { __lp_lux_static_array_init(0); }
	//Sets the size of the array and allocates it, without inizializing the elements
	inline __vectorcall LuxArray(const uint64 vInitSize) { __lp_lux_static_array_init(vInitSize); }


	//Initializes the array using a list of elements, automatically converting it to the right type
	template<class inType>
	inline __vectorcall LuxArray(const std::initializer_list<inType> vElements) {
		__lp_lux_static_array_init(vElements.size());
		for (int i = 0; i < vElements.end() - vElements.begin(); ++i) __lp_data[i] = (inType) * (vElements.begin() + i);
	}
	//Initializes the array using a list of elements of the same type
	inline __vectorcall LuxArray(const std::initializer_list<type> vElements) {
		__lp_lux_static_array_init(vElements.size());
		memcpy(begin(), vElements.begin(), ((vElements.size() * sizeof(type))));
	}


	//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
	//*   pArray: a pointer to the container object
	template<class elmType>
	inline __vectorcall LuxArray(const LuxContainer<elmType>* pArray) {
		__lp_lux_static_array_init(pArray->end() - pArray->begin());
		for (int i = 0; i < pArray->end() - pArray->begin(); ++i) __lp_data[i] = (elmType) * (pArray->begin() + i);
	}
	#undef __lp_lux_static_array_init




	// Get, set, begin, end ---------------------------------------------------------------------------------------------------------------------//




	inline uint64 __vectorcall size() const { return __lp_size; }
	inline type* __vectorcall data() const { return __lp_data; }

	inline type& __vectorcall operator[](const uint64 vIndex) const { return __lp_data[vIndex]; }
	inline type* __vectorcall begin() const override { return &__lp_data[0]; }
	inline type* __vectorcall end() const override { return &__lp_data[__lp_size - 1]; }




	// Resize -----------------------------------------------------------------------------------------------------------------------------------//




	//Resizes the array without initializing the new elements
	//*   vNewSize: the new size of the array
	//*   Returns the new size. -1 if the size is invalid
	inline uint64 __vectorcall resize(const uint64 vNewSize) {
		if (vNewSize < 0) return -1;
		__lp_size = vNewSize;
		__lp_data = (type*)realloc(__lp_data, sizeof(type) * __lp_size);
		return vNewSize;
	}

	//Resizes the array and initializes the new elements with a value
	//*   vNewSize: the new size of the array
	//*   vInitValue: the value to use to initialize the new elements
	//*   Returns the new size. -1 if the size is invalid
	inline uint64 __vectorcall resize(const uint64 vNewSize, const type vInitValue) {
		if (vNewSize < 0) return -1;
		uint64 oldSize = __lp_size;
		resize(vNewSize);
		if (vNewSize > oldSize) for (uint64 i = oldSize; i < vNewSize; ++i) __lp_data[i] = vInitValue;
		return vNewSize;
	}
};
#undef __lp_lux_static_array_init