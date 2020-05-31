#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include <stdlib.h>
#include <initializer_list>
#include <assert.h>
#include <algorithm>


//A static array that knows its size. It can be resized, but it doesn't have add or remove methods. For a dynamically sized array, use LuxArray
template <class type>
class LuxStaticArray {
public:
	type* data;		//Elements of the array
	uint64 size;	//Size of the array
	#define __lux_static_array_init(_size) size = _size; data = (type*)malloc(sizeof(type) * size)


	//Creates an array with no elements
	LuxStaticArray() { __lux_static_array_init(0); }
	//Sets the size of the array and allocates it, without inizializing the elements
	LuxStaticArray(uint64 initSize) { __lux_static_array_init(initSize); }

	//Initializes the array using a list of elements of the same type
	LuxStaticArray(std::initializer_list<type> c) {
		__lux_static_array_init(c.size());
		std::copy(c.begin(), c.end(), data);
	}

	//Initializes the array using a container object and converting each element in the array's type. The input container must have a begin() and an end() function
	//*   in: a pointer to the container object
	template <class inType>
	LuxStaticArray(void* in) {
		inType* _in = (inType*)_in;
		__lux_static_array_init(_in->end() - _in->begin());
		for (int i = 0; i < _in->end() - _in->begin(); i++) data[i] = (type)*_in->begin() + i;
	}


	inline type& operator [](uint64 index) { return data[index]; }
	inline type* begin() { return &data[0]; }
	inline type* end() { return &data[size - 1]; }

	//Resizes the array
	inline void resize(uint64 newSize) {
		size = newSize;
		data = (type*)realloc(data, sizeof(type) * size);
	}

	//Resizes the array and initializes the new elements with the val parameter's value
	inline void resize(uint64 newSize, type val) {
		uint64 oldSize = size;
		resize(newSize);
		if (newSize > oldSize) for (uint64 i = oldSize; i < newSize; i++) data[i] = val;
	}
};