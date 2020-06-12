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

	fvec3 pos;
	fvec3 rot;
	fvec3 scl;
	LuxGpuBuffer mainGpuData;

	ObjectGeometry geometry;
	ObjectPhysics physics;


	LuxObject(std::string _name = "Undefined", fvec3 _pos = { 0, 0, 0 }, fvec3 _rot = { 0, 0, 0 }, fvec3 _scl = { 1, 1, 1 }) {
		name = _name;
		pos = _pos;
		rot = _rot;
		scl = _scl;
	}

};