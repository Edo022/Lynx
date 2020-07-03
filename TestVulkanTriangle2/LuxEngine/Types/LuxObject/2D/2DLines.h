#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include <vulkan/vulkan.h>




//Base class for 2D lines
struct LuxObjectLine_base2 : public LuxObject2D_base1 {
	LuxObjectLine_base2() { objectType = LUX_OBJECT_TYPE_LINE_2D__BASE; }

	vec2float32 pos0{ 0,0 };		//Position of the first vertex
	vec2float32 pos1{ 0,0 };		//Position of the second vertex
};








//A 2D line with a start and end position, a wd and an RGBA color for each vertex
//Use LuxObjectLineCT, LuxObjectLineCC or LuxObjectLineCCT for better performances with constant tickness or color lines
struct LuxObjectLine : public LuxObjectLine_base2 {
	LuxObjectLine() { objectType = LUX_OBJECT_TYPE_LINE_2D; }

	vec4float32 col0{ 0,0,0,0 };	//Color of the first vertex
	vec4float32 col1{ 0,0,0,0 };	//Color of the second vertex
	float32 wd0{ 1 };				//width of the first vertex
	float32 wd1{ 1 };				//width of the second vertex
};




//Variant of LuxObjectLine. This version has constant wd
struct LuxObjectLineCT : public LuxObjectLine_base2 {
	LuxObjectLineCT() { objectType = LUX_OBJECT_TYPE_LINE_2D_CT; }

	vec4float32 col0{ 0,0,0,0 };	//Color of the first vertex
	vec4float32 col1{ 0,0,0,0 };	//Color of the second vertex
	float32 wd{ 1 };				//width of the line
};




//Variant of LuxObjectLine. This version has constant color
struct LuxObjectLineCC : public LuxObjectLine_base2 {
	LuxObjectLineCC() { objectType = LUX_OBJECT_TYPE_LINE_2D_CC; }

	vec4float32 col{ 0,0,0,0 };		//Color of the line
	float32 wd0{ 1 };				//width of the first vertex
	float32 wd1{ 1 };				//width of the second vertex
};




//Variant of LuxObjectLine. This version has constant color and wd
struct LuxObjectLineCCT : public LuxObjectLine_base2 {
	LuxObjectLineCCT() { objectType = LUX_OBJECT_TYPE_LINE_2D_CCT; }

	vec4float32 col{ 0,0,0,0 };		//Color of the line
	float32 wd{ 1 };				//width of the line
};




//Variant of LuxObjectLine. This version has constant color and wd
struct LuxDynamic_LuxObjectLineCCT : public LuxObjectLine_base2 {
	LuxDynamic_LuxObjectLineCCT() { objectType = LUX_OBJECT_TYPE_LINE_2D_CCT; }

	void hhh_() {
		x0 = ((int32*)cellPtr) + 0;
		y0 = ((int32*)cellPtr) + 1;
		x1 = ((int32*)cellPtr) + 2;
		y1 = ((int32*)cellPtr) + 3;

		col = (vec4uint32*)(((int32*)cellPtr) + 4);

		wd = ((float32*)cellPtr) + 8;
	}

	int32* x0 = nullptr;
	int32* y0 = nullptr;
	int32* x1 = nullptr;
	int32* y1 = nullptr;

	vec4uint32* col = nullptr;

	float32* wd = nullptr;
};


//TODO dual camera