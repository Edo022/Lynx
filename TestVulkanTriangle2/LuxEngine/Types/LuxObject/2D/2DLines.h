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

	void setCol(vec4float32 col) {
		uint32* map = (uint32*)cellPtr;
		map[4] = col.x;
		map[5] = col.y;
		map[6] = col.z;
		map[7] = col.w;
	}

	void setWd(float32 wd) {
		float32* map = (float32*)cellPtr;
		map[8] = wd;
	}

	void setX0(int32 x0) {
		int32* map = (int32*)cellPtr;
		map[0] = x0;
	}
	void setY0(int32 y0) {
		int32* map = (int32*)cellPtr;
		map[1] = y0;
	}

	void setX1(int32 x1) {
		int32* map = (int32*)cellPtr;
		map[2] = x1;
	}
	void setY1(int32 y1) {
		int32* map = (int32*)cellPtr;
		map[3] = y1;
	}
};


//TODO dual camera