#pragma once

#include "ObjectGeometry.h"
#include "ObjectPhysics.h"
#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Types/Containers/LuxString.h"



struct LuxObject {
	LuxString name;
	uint64 ID;

	vec3float pos;
	vec3float rot;
	vec3float scl;
	LuxCell mainGpuData;

	ObjectGeometry geometry;
	ObjectPhysics physics;


	inline LuxObject(const LuxString vName = "Undefined", const vec3float vPos = { 0, 0, 0 }, const vec3float vRot = { 0, 0, 0 }, const vec3float vScl = { 1, 1, 1 }) {
		name = vName;
		pos = vPos;
		rot = vRot;
		scl = vScl;
	}
};