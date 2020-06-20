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


	LuxObject(std::string _name = "Undefined", vec3float _pos = { 0, 0, 0 }, vec3float _rot = { 0, 0, 0 }, vec3float _scl = { 1, 1, 1 }) {
		name = _name;
		pos = _pos;
		rot = _rot;
		scl = _scl;
	}

};