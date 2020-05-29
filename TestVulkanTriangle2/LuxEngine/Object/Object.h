#pragma once

#include "ObjectGeometry.h"
#include "ObjectPhysics.h"
#include <string>
#include "LuxEngine/Types/Vectors/Vectors.h"


struct LuxObject {
	std::string name;
	uint64 ID;

	lvec3 pos;
	dvec3 rot;
	dvec3 scl;

	ObjectGeometry geometry;
	ObjectPhysics physics;


	LuxObject(std::string _name = "Undefined", lvec3 _pos = { 0, 0, 0 }, dvec3 _rot = { 0, 0, 0 }, dvec3 _scl = { 1, 1, 1 }) {
		name = _name;
		pos = _pos;
		rot = _rot;
		scl = _scl;
	}

};