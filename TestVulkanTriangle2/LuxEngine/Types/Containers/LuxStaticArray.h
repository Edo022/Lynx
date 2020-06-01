#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include <initializer_list>


//A static array that knows its size. It can be resized, but it doesn't have add or remove methods
template <class type>
class LuxStaticArray {
public:
	type* __lp_data;	//Elements of the array
	uint64 __lp_size;	//Size of the array
	#define __lp_lux_static_array_init(_size) __lp_size = _size; __lp_data = (type*)malloc(sizeof(type) * __lp_size)


	//Creates an array with no elements
	LuxStaticArray() { __lp_lux_static_array_init(0); }
	//Sets the size of the array and allocates it, without inizializing the elements
	LuxStaticArray(uint64 initSize) { __lp_lux_static_array_init(initSize); }

	//Initializes the array using a list of elements of the same type
	LuxStaticArray(std::initializer_list<type> c) {
		__lp_lux_static_array_init(c.size());
		std::copy(c.begin(), c.end(), __lp_data);
	}

	////Initializes the array using a container object and converting each element in the array's type. The input container must have a begin() and an end() function
	////*   in: a pointer to the container object
	//template<class inType>
	//LuxStaticArray(inType in) {
	//	__lp_lux_static_array_init(((inType)in).end() - in.begin());
	//	for (int i = 0; i < in.end() - in.begin(); i++) __lp_data[i] = (type) * (in.begin() + i);
	//}

	inline uint64 size() { return __lp_size; };
	inline type* data() { return __lp_data; };

	inline type& operator [](uint64 index) { return __lp_data[index]; }
	inline type* begin() { return &__lp_data[0]; }
	inline type* end() { return &__lp_data[__lp_size - 1]; }

	//Resizes the array. Returns the new size
	inline uint64 resize(uint64 newSize) {
		__lp_size = newSize;
		__lp_data = (type*)realloc(__lp_data, sizeof(type) * __lp_size);
		return newSize;
	}

	//Resizes the array and initializes the new elements with the val parameter's value
	inline void resize(uint64 newSize, type val) {
		uint64 oldSize = __lp_size;
		resize(newSize);
		if (newSize > oldSize) for (uint64 i = oldSize; i < newSize; i++) __lp_data[i] = val;
	}
};