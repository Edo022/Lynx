#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include <vulkan/vulkan.h>




//Base class for 2D lines
struct LuxObjectLine_base2 : public LuxObject2D_base1 {
	LuxObjectLine_base2() { objectType = LUX_OBJECT_TYPE_LINE_2D__BASE; }

	vec2float32 posA{ 0,0 };		//Position of the first vertex
	vec2float32 posB{ 0,0 };		//Position of the second vertex
};








//A 2D line with a start and end position, a thickness and an RGBA color for each vertex
//Use LuxObjectLineCT, LuxObjectLineCC or LuxObjectLineCCT for better performances with constant tickness or color lines
struct LuxObjectLine : public LuxObjectLine_base2 {
	LuxObjectLine() { objectType = LUX_OBJECT_TYPE_LINE_2D; }
	vec4float32 colA{ 0,0,0,0 };	//Color of the first vertex
	vec4float32 colB{ 0,0,0,0 };	//Color of the second vertex
	float32 thicknessA{ 1 };		//Thickness of the first vertex
	float32 thicknessB{ 1 };		//Thickness of the second vertex
};




//Variant of LuxObjectLine. This version has constant thickness
struct LuxObjectLineCT : public LuxObjectLine_base2 {
	LuxObjectLineCT() { objectType = LUX_OBJECT_TYPE_LINE_2D_CT; }

	vec4float32 colA{ 0,0,0,0 };	//Color of the first vertex
	vec4float32 colB{ 0,0,0,0 };	//Color of the second vertex
	float32 thickness{ 1 };			//Thickness of the line
};




//Variant of LuxObjectLine. This version has constant color
struct LuxObjectLineCC : public LuxObjectLine_base2 {
	LuxObjectLineCC() { objectType = LUX_OBJECT_TYPE_LINE_2D_CC; }

	vec4float32 col{ 0,0,0,0 };		//Color of the line
	float32 thicknessA{ 1 };		//Thickness of the first vertex
	float32 thicknessB{ 1 };		//Thickness of the second vertex
};




//Variant of LuxObjectLine. This version has constant color and thickness
struct LuxObjectLineCCT : public LuxObjectLine_base2 {
	LuxObjectLineCCT() { objectType = LUX_OBJECT_TYPE_LINE_2D_CCT; }

	vec4float32 col{ 0,0,0,0 };		//Color of the line
	float32 thickness{ 1 };			//Thickness of the line
};


//TODO dual camera