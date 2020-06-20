#pragma once

#include "ObjectGeometry.h"
#include "ObjectPhysics.h"
#include <string>
#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"



struct LuxObject {
	std::string name;
	uint64 ID;

	vec3float pos;
	vec3float rot;
	vec3float scl;
	LuxCell mainGpuData;

	ObjectGeometry geometry;
	ObjectPhysics physics;


	//TODO use LuxString
	inline __vectorcall LuxObject(const std::string vName = "Undefined", const vec3float vPos = { 0, 0, 0 }, const vec3float vRot = { 0, 0, 0 }, const vec3float vScl = { 1, 1, 1 }) {
		name = vName;
		pos = vPos;
		rot = vRot;
		scl = vScl;
	}
};