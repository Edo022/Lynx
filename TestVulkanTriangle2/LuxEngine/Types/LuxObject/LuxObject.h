#pragma once

#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Types/Containers/LuxString.h"




enum LuxObjectType : int32 {
	LUX_OBJECT_TYPE__BASE = -1,

	LUX_OBJECT_TYPE_3D__BASE = -2,
	LUX_OBJECT_TYPE_2i3D__BASE = -3,
	LUX_OBJECT_TYPE_2D__BASE = -4,
	LUX_OBJECT_TYPE_1D__BASE = -5,

	LUX_OBJECT_TYPE_RENDER_SPACE_2D = 1,
	LUX_OBJECT_TYPE_RENDER_SPACE_3D = 2,

	LUX_OBJECT_TYPE_2D_LINE = 3,
};










//Base class for render objects
struct LuxObject_base0 {
	LuxObjectType objectType;
	LuxObject_base0() { objectType = LUX_OBJECT_TYPE__BASE; }

	LuxString name{ "" };			//The name of the object. 
	static uint64 lastID;			//#LLID LOS000 the last assigned ID of a LuxObject 
	uint64 ID{ ++lastID };			//A unique ID that indentifies the object

	LuxCell gpuCell{ (uint64)-1 };	//GPU memory containing the small data of the object
	void* cellPtr = nullptr;		//Pointer to the GPU memory cell
	virtual void initPtrs() = 0;
};






//3D object in 3D space
struct LuxObject3D_base1 : public LuxObject_base0 {
	LuxObject3D_base1() { objectType = LUX_OBJECT_TYPE_3D__BASE; }

	vec3f32 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
	vec3f32 org{ 0, 0, 0 };			//Origin of the object
	float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
	vec3f32 rot{ 0, 0, 0 };			//Rotation of the object
	vec3f32 scl{ 0, 0, 0 };			//Scale of the object
};




//Base class for 2D objects with 3D properties (they can be used in both 2D and 3D spaces)
struct LuxObject2i3D_base1 : public LuxObject_base0 {
	LuxObject2i3D_base1() { objectType = LUX_OBJECT_TYPE_2i3D__BASE; }

	vec3f32 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
	vec3f32 org{ 0, 0, 0 };			//Origin of the object
	float32 wIndex{ 0 };			//Index of the object for 3D space
	float32 zIndex{ 0 };			//Index of the object for 2D space
	vec3f32 rot{ 0, 0, 0 };			//Rotation of the object
	vec2f32 scl{ 0, 0 };			//Scale of the object
};




//Base class for 2D objects in 2D space
struct LuxObject2D_base1 : public LuxObject_base0 {
	LuxObject2D_base1() { objectType = LUX_OBJECT_TYPE_2D__BASE; }

	vec2f32 pos{ 0, 0 };			//Position of the object. The position is relative to the origin of the object
	vec2f32 org{ 0, 0 };			//Origin of the object
	float32 zIndex{ 0 };			//Index of the object. Objects with higher zIndex will be rendered on top of others
	float32 rot{ 0 };				//Rotation of the object
	vec2f32 scl{ 0, 0 };			//Scale of the object
};




//Base class for 1D objects in 1D space
struct luxObject1D_base1 : public LuxObject_base0 {
	luxObject1D_base1() { objectType = LUX_OBJECT_TYPE_1D__BASE; }

	float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
	float32 org{ 0 };				//Origin of the object
	float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
	float32 scl{ 0 };				//Scale of the object
};