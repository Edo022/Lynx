#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include <vulkan/vulkan.h>




//A bidimensional line with interpolated color and width
//Lines with the same color or width have better performance
//Perfectly vertical or horyzontal lines also have better performance
//Lines with size 0 or alpha 0 are not rendered
struct LuxDynamic_LuxObjectLineCCT : public LuxObject2D_base1 {
	LuxDynamic_LuxObjectLineCCT() { objectType = LUX_OBJECT_TYPE_2D_LINE; }

	void initPtrs() {
		x0 = ((int32*)cellPtr) + 0;
		y0 = ((int32*)cellPtr) + 1;
		x1 = ((int32*)cellPtr) + 2;
		y1 = ((int32*)cellPtr) + 3;

		col0 = (vec4float32*)(((int32*)cellPtr) + 4);
		col1 = (vec4float32*)(((int32*)cellPtr) + 8);

		wd0 = ((float32*)cellPtr) + 12;
		wd1 = ((float32*)cellPtr) + 13;
	}

	int32* x0{ nullptr };			//The x position of the first point
	int32* y0{ nullptr };			//The y position of the first point
	int32* x1{ nullptr };			//The x position of the second point
	int32* y1{ nullptr };			//The y position of the second point

	vec4float32* col0{ nullptr };	//Color of the first point
	vec4float32* col1{ nullptr };	//Color of the second point

	float32* wd0{ nullptr };		//Width of the first point
	float32* wd1{ nullptr };		//Width of the second point
};


//TODO dual camera