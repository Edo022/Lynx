#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include <vulkan/vulkan.h>




//Base class for 2D lines
struct LuxObjectLineBase : public LuxObject2D {
	vec2float32 posA;	//Position of the first vertex
	vec2float32 posB;	//Position of the second vertex
};




//A 2D line with a start and end position, a thickness and an RGBA color for each vertex
//Use LuxObjectLineCT, LuxObjectLineCC or LuxObjectLineCCT for better performances with constant tickness or color lines
struct LuxObjectLine : public LuxObjectLineBase {
	vec4float32 colA;	//Color of the first vertex
	vec4float32 colB;	//Color of the second vertex
	float32 thicknessA;	//Thickness of the first vertex
	float32 thicknessB;	//Thickness of the second vertex
};


//Variant of LuxObjectLine. This version has constant thickness
struct LuxObjectLineCT : public LuxObjectLineBase {
	vec4float32 colA;	//Color of the first vertex
	vec4float32 colB;	//Color of the second vertex
	float32 thickness;	//Thickness of the line
};

//Variant of LuxObjectLine. This version has constant color
struct LuxObjectLineCC : public LuxObjectLineBase {
	vec4float32 col;	//Color of the line
	float32 thicknessA;	//Thickness of the first vertex
	float32 thicknessB;	//Thickness of the second vertex
};

//Variant of LuxObjectLine. This version has constant color and thickness
struct LuxObjectLineCCT : public LuxObjectLineBase {
	vec4float32 col;	//Color of the line
	float32 thickness;	//Thickness of the line
};


//TODO dual camera