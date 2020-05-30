#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include <stdlib.h>


//Just a normal array that knows its size
template <class type>
class LuxStaticArray {
public:
	type* data;
	uint64 size;

	LuxStaticArray(uint64 initSize) {
		size = initSize;
		data = (type*)malloc(sizeof(type) * size);
	}

	inline type& operator [](uint64 index) {
		return data[index];
	}

	inline type* begin() {
		return &data[0];
	}
	inline type* end() {
		return &data[size-1];
	}
};