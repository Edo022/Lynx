#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include <vulkan/vulkan.h>



namespace lux::obj {
	//A bidimensional line with interpolated color and width
	//Lines with size 0 or alpha 0 are not rendered
	struct Line2D : public Base2D {
		Line2D() {
			objectType = LUX_OBJECT_TYPE_2D_LINE;	//Set object type
			shaderName = "Line2D";					//Set rendering shader
			this->allocate();						//Allocate object data
		}


		//Updates the object data in the GPU memory
		//This function should be called only after the object is begin spawned
		void update() final override {
			//if(parent != nullptr) parent->setChildLimits(childIndex);
			vec2i32 h = { 1920, 1080/2 };

			vec2f32 hh = dist2D(minLim, maxLim);
			vec2f32 hh1 = (p0 - h);
			vec2f32 hh2 = (p0 - h) * dist2D(minLim, maxLim);
			*(vec2i32*)((int8*)cellPtr + 0) = (p0 - h) * dist2D(minLim, maxLim) + h;		//    8    |    0  - 7     |    0 +
			*(vec2i32*)((int8*)cellPtr + 8) = (p1 - h) * dist2D(minLim, maxLim) + h;		//    8    |    8  - 15    |    0
			*(vec4f32*)((int8*)cellPtr + 16) = col0;	//    16   |    16 - 31    |    1 +
			*(vec4f32*)((int8*)cellPtr + 32) = col1;	//    16   |    32 - 47    |    2 +
			*(float32*)((int8*)cellPtr + 48) = wd0;		//    4    |    48 - 51    |    3 +
			*(float32*)((int8*)cellPtr + 52) = wd1;		//    4    |    52 - 55    |    3
			//7x trailing padding						//    7    |    56 - 63

			//recalculateBoundary( );
			//recalculate boundary function in object base
		}

		inline int32 getCellSize() const final override { return 56; }

		vec2i32 p0{ 0, 0 };			//First point of the line
		vec2i32 p1{ 0, 0 };			//Second point of the line
		vec4f32 col0{ 0, 0, 0, 0 };	//Color of the first point
		vec4f32 col1{ 0, 0, 0, 0 };	//Color of the second point
		float32 wd0{ 0 };			//Width of the first point
		float32 wd1{ 0 };			//Width of the second point
	};
}

//TODO dual camera