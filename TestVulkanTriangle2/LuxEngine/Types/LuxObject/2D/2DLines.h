#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include <vulkan/vulkan.h>



namespace lux::obj {
	//A bidimensional line with interpolated color and width
	//Lines with the same color or width have better performance
	//Perfectly vertical or horyzontal lines also have better performance
	//Lines with size 0 or alpha 0 are not rendered
	struct Line2D : public Base2D {
		Line2D() { objectType = LUX_OBJECT_TYPE_2D_LINE; shaderName = "Line2D"; }

		//Initializes the pointers to the shared memory
		//This is needed to read or write data in the object
		//This function should be called only after the object is begin spawned
		void initPtrs() final override {
			p0 = (vec2i32*)((int8*)cellPtr + 0);		//    8    |    0  - 7     |    0 +
			p1 = (vec2i32*)((int8*)cellPtr + 8);		//    8    |    8  - 15    |    0
			col0 = (vec4f32*)((int8*)cellPtr + 16);		//    16   |    16 - 31    |    1 +
			col1 = (vec4f32*)((int8*)cellPtr + 32);		//    16   |    32 - 47    |    2 +
			wd0 = (float32*)((int8*)cellPtr + 48);		//    4    |    48 - 51    |    3 +
			wd1 = (float32*)((int8*)cellPtr + 52);		//    4    |    52 - 55    |    3
		}

		inline int32 getCellSize() final override { return 56; }

		vec2i32* p0{ nullptr };		//First point of the line
		vec2i32* p1{ nullptr };		//Second point of the line
		vec4f32* col0{ nullptr };	//Color of the first point
		vec4f32* col1{ nullptr };	//Color of the second point
		float32* wd0{ nullptr };	//Width of the first point
		float32* wd1{ nullptr };	//Width of the second point
	};
}

//TODO dual camera