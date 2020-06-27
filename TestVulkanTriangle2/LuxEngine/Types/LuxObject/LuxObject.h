#pragma once

#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Types/Containers/LuxString.h"



enum LuxObjectType : int32 {
	LUX_OBJECT_TYPE__BASE,

	LUX_OBJECT_TYPE_3D__BASE,
	LUX_OBJECT_TYPE_2i3D__BASE,
	LUX_OBJECT_TYPE_2D__BASE,
	LUX_OBJECT_TYPE_1D__BASE,

	LUX_OBJECT_TYPE_RENDER_SPACE_2D,
	LUX_OBJECT_TYPE_RENDER_SPACE_3D,

	LUX_OBJECT_TYPE_LINE_2D__BASE,
	LUX_OBJECT_TYPE_LINE_2D,
	LUX_OBJECT_TYPE_LINE_2D_CC,
	LUX_OBJECT_TYPE_LINE_2D_CT,
	LUX_OBJECT_TYPE_LINE_2D_CCT
};




//Base class for render objects
struct LuxObject_base0 {
	static const LuxObjectType objectType = LUX_OBJECT_TYPE__BASE;
	//TODO optimize name. don't use strings when not needed
	LuxString name = "";	//The name of the object. 
	uint64 ID = -1;			//A unique ID that indentifies the object. IDs are exclusive to spawned objects. <renderSpace>.luxSpawnObject(*<object>)
};


//3D object in 3D space
struct LuxObject3D_base1 : public LuxObject_base0 {
	static const LuxObjectType objectType = LUX_OBJECT_TYPE_3D__BASE;
	vec3float32 pos = { 0, 0, 0 };	//Position of the object. The position is relative to the origin of the object
	vec3float32 org = { 0, 0, 0 };	//Origin of the object
	float32 wIndex = 0;				//Index of the object. Objects with higher wIndex will be rendered on top of others
	vec3float32 rot = { 0, 0, 0 };	//Rotation of the object
	vec3float32 scl = { 0, 0, 0 };	//Scale of the object
};


//Base class for 2D objects with 3D properties (they can be used in both 2D and 3D spaces)
struct LuxObject2i3D_base1 : public LuxObject_base0 {
	static const LuxObjectType objectType = LUX_OBJECT_TYPE_2i3D__BASE;
	vec3float32 pos = { 0, 0, 0 };	//Position of the object. The position is relative to the origin of the object
	vec3float32 org = { 0, 0, 0 };	//Origin of the object
	float32 wIndex = 0;				//Index of the object for 3D space
	float32 zIndex = 0;				//Index of the object for 2D space
	vec3float32 rot = { 0, 0, 0 };	//Rotation of the object
	vec2float32 scl = { 0, 0 };		//Scale of the object
};


//Base class for 2D objects in 2D space
struct LuxObject2D_base1 : public LuxObject_base0 {
	static const LuxObjectType objectType = LUX_OBJECT_TYPE_2D__BASE;
	vec2float32 pos = { 0, 0 };		//Position of the object. The position is relative to the origin of the object
	vec2float32 org = { 0, 0 };		//Origin of the object
	float32 zIndex = 0;				//Index of the object. Objects with higher zIndex will be rendered on top of others
	float32 rot = 0;				//Rotation of the object
	vec2float32 scl = { 0, 0 };		//Scale of the object
};


//Base class for 1D objects in 1D space
struct luxObject1D_base1 : public LuxObject_base0 {
	static const LuxObjectType objectType = LUX_OBJECT_TYPE_1D__BASE;
	float32 pos = 0;				//Position of the object. The position is relative to the origin of the object
	float32 org = 0;				//Origin of the object
	float32 yIndex = 0;				//Index of the object. Objects with higher yIndex will be rendered on top of others
	float32 scl = 0;				//Scale of the object
};