#pragma once

#include "LuxObjectGeometry.h"
#include "LuxObjectPhysics.h"
#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Types/Containers/LuxString.h"


//TODO maybe useless. use a .lux file instead of this
enum LuxObjectType : int32 {
	LUX_OBJECT_TYPE_UNDEFINED = -1,
	LUX_OBJECT_TYPE_GEOMETRY = 0 //TODO
};


//Base class for render objects
struct LuxObject {
	LuxString name;
	uint64 ID;
};




//3D object in 3D space
struct LuxObject3D : public LuxObject {
	vec3float32 pos;	//Position of the object
	float32 wIndex;		//Index of the object. Objects with higher wIndex will be rendered on top of others
	vec3float32 rot;	//Rotation of the object
	vec3float32 scl;	//Scale of the object
};


//2D object with 3D properties. It can be used in both 2D and 3D spaces
struct LuxObjectt2p5D : public LuxObject {
	vec3float32 pos;	//Position of the object
	float32 wIndex;		//Index of the object for 3D space
	float32 zIndex;		//Index of the object for 2D space
	vec3float32 rot;	//Rotation of the object
	vec2float32 scl;	//Scale of the object
};


//2D object in 2D space
struct LuxObject2D : public LuxObject {
	vec2float32 pos;	//Position of the object. It goes from 0.0, 0.0 to 1.0, 1.0
	float32 zIndex;		//Index of the object. Objects with higher zIndex will be rendered on top of others
	float32 rot;		//Rotation of the object
	vec2float32 scl;	//Scale of the object
};