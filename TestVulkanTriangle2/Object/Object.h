#pragma once

#include "ObjectGeometry.h"
#include "ObjectPhysics.h"
#include <string>
#include "Types/Vectors/Vectors.h"


struct LuxObject {
	std::string name;
	uint64_t ID;

	vec3 pos;
	dvec3 rot;
	dvec3 scl;

	ObjectGeometry geometry;
	ObjectPhysics physics;


	LuxObject(std::string _name = "Undefined", vec3 _pos = { 0,0,0 }, dvec3 _rot = { 0,0,0 }, dvec3 _scl = { 10,10,10 }) {
		name = _name;
		pos = _pos;
		rot = _rot;
		scl = _scl;
	}

};




//Loads an .OBJ file into the object's geometry, overwriting it
//	object		a pointer to the LuxObject where to load the obj
//	objPath		the path to the .OBJ file
//	Returns true if the obj loading succeed. false if the file is not found or invalid
static bool luxObjectLoadObj(LuxObject* object, std::string objPath) {
	//TODO use CRAVector
	object->geometry.vertices.clear();
	object->geometry.indices.clear();

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str())) {
		printf("%s\n", (warn + err).c_str());
		return false;
	}



	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * (int64)(index.vertex_index) + 0],
				attrib.vertices[3 * (int64)(index.vertex_index) + 1],
				attrib.vertices[3 * (int64)(index.vertex_index) + 2]
			};
			vertex.texCoord = {
				attrib.texcoords[2 * (int64)(index.texcoord_index) + 0],
				attrib.texcoords[2 * (int64)(index.texcoord_index) + 1]
			};
			vertex.texCoord = {
				attrib.texcoords[2 * (int64)(index.texcoord_index) + 0],
				1.0f - attrib.texcoords[2 * (int64)(index.texcoord_index) + 1]		//flip z
			};
			vertex.color = { 1.0f, 1.0f, 1.0f };

			object->geometry.vertices.add(vertex);
			object->geometry.indices.add(object->geometry.indices.size());
		}
	}
	return true;
}
