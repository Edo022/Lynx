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

		void update() final override;
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