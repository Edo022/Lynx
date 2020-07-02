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

	LUX_OBJECT_TYPE_LINE_2D__BASE = -6,
	LUX_OBJECT_TYPE_LINE_2D = 3,
	LUX_OBJECT_TYPE_LINE_2D_CC = 4,
	LUX_OBJECT_TYPE_LINE_2D_CT = 5,
	LUX_OBJECT_TYPE_LINE_2D_CCT = 6
};




enum class LuxObjectUpdate : uint64 {
	LUX_OBJECT_UPDATE_POS_X	=	0x1,
	LUX_OBJECT_UPDATE_POS_Y	=	0x2,
	LUX_OBJECT_UPDATE_POS_Z	=	0x4,
	LUX_OBJECT_UPDATE_POS_W	=	0x8,
							  
	LUX_OBJECT_UPDATE_ROT_X	=	0x10,
	LUX_OBJECT_UPDATE_ROT_Y	=	0x20,
	LUX_OBJECT_UPDATE_ROT_Z	=	0x40,
							  
	LUX_OBJECT_UPDATE_SCL_X	=	0x80,
	LUX_OBJECT_UPDATE_SCL_Y	=	0x100,
	LUX_OBJECT_UPDATE_SCL_Z	=	0x200,

	LUX_OBJECT_UPDATE_Y_INDEX = 0x400,
	LUX_OBJECT_UPDATE_Z_INDEX = 0x800,
	LUX_OBJECT_UPDATE_W_INDEX = 0x1000,

	LUX_OBJECT_UPDATE_WD =		0x2000,
	LUX_OBJECT_UPDATE_WD0 =		0x4000,
	LUX_OBJECT_UPDATE_WD1 =		0x8000,

	// 16 //

	LUX_OBJECT_UPDATE_COL =		0x10000,
	LUX_OBJECT_UPDATE_COL0 =	0x20000,
	LUX_OBJECT_UPDATE_COL1 =	0x40000,

};






//Base class for render objects
struct LuxObject_base0 {
	LuxObjectType objectType;
	LuxObject_base0() { objectType = LUX_OBJECT_TYPE__BASE; }

	static uint64 lastID;				//#LLID LOS000 the last assigned ID of a LuxObject 
	//TODO optimize name. don't use strings when not needed
	LuxString name{ "" };				//The name of the object. 
	uint64 ID{ ++lastID };				//A unique ID that indentifies the object
	LuxCell gpuCell{ (uint64)-1 };		//GPU memory containing the small data of the object

	//This function update the data in the GPU
	//*   vUpdates: the struct members to update
	//*       The struct you are updating must contain those members
	//*   Returns 0 if the members are updated correctly
	//*       1 if the struct has no corresponding GPU memory (This happens when you create an object but don't spawn it)
	//*       2 if only some members are updated becaouse the others are invalid
	//*       -1 if all the members are invalid
	//*       -2 if an unknown error occurs
	int updateFromCPU(const LuxObjectUpdate vUpdates) {
		engine.updates.push_back(vUpdates);
	}

	uint64 EID{ (uint64)-1 };			//A unique ID that only spawned objects have
};






//3D object in 3D space
struct LuxObject3D_base1 : public LuxObject_base0 {
	LuxObject3D_base1() { objectType = LUX_OBJECT_TYPE_3D__BASE; }

	vec3float32 pos{ 0, 0, 0 };	//Position of the object. The position is relative to the origin of the object
	vec3float32 org{ 0, 0, 0 };	//Origin of the object
	float32 wIndex{ 0 };		//Index of the object. Objects with higher wIndex will be rendered on top of others
	vec3float32 rot{ 0, 0, 0 };	//Rotation of the object
	vec3float32 scl{ 0, 0, 0 };	//Scale of the object
};




//Base class for 2D objects with 3D properties (they can be used in both 2D and 3D spaces)
struct LuxObject2i3D_base1 : public LuxObject_base0 {
	LuxObject2i3D_base1() { objectType = LUX_OBJECT_TYPE_2i3D__BASE; }

	vec3float32 pos{ 0, 0, 0 };	//Position of the object. The position is relative to the origin of the object
	vec3float32 org{ 0, 0, 0 };	//Origin of the object
	float32 wIndex{ 0 };		//Index of the object for 3D space
	float32 zIndex{ 0 };		//Index of the object for 2D space
	vec3float32 rot{ 0, 0, 0 };	//Rotation of the object
	vec2float32 scl{ 0, 0 };	//Scale of the object
};




//Base class for 2D objects in 2D space
struct LuxObject2D_base1 : public LuxObject_base0 {
	LuxObject2D_base1() { objectType = LUX_OBJECT_TYPE_2D__BASE; }

	vec2float32 pos{ 0, 0 };	//Position of the object. The position is relative to the origin of the object
	vec2float32 org{ 0, 0 };	//Origin of the object
	float32 zIndex{ 0 };		//Index of the object. Objects with higher zIndex will be rendered on top of others
	float32 rot{ 0 };			//Rotation of the object
	vec2float32 scl{ 0, 0 };	//Scale of the object
};




//Base class for 1D objects in 1D space
struct luxObject1D_base1 : public LuxObject_base0 {
	luxObject1D_base1() { objectType = LUX_OBJECT_TYPE_1D__BASE; }

	float32 pos{ 0 };			//Position of the object. The position is relative to the origin of the object
	float32 org{ 0 };			//Origin of the object
	float32 yIndex{ 0 };		//Index of the object. Objects with higher yIndex will be rendered on top of others
	float32 scl{ 0 };			//Scale of the object
};