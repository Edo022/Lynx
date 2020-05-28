#pragma once

#include "ObjectGeometry.h"
#include "ObjectPhysics.h"
#include <string>
#include "Types/Vectors/Vectors.h"


extern int64 LAST_OBJECT_ID;

enum MrgrUpdateObjectBits {
	MDR_UPDATE_OBJECT_ROT_X_BIT	= 0b00000001,		//Update only the object's x rotation
	MDR_UPDATE_OBJECT_ROT_Y_BIT	= 0b00000010,		//Update only the object's y rotation
	MDR_UPDATE_OBJECT_ROT_Z_BIT	= 0b00000100,		//Update only the object's z rotation
	MDR_UPDATE_OBJECT_ROT_ALL_BITS	= 0b00000111,		//Update the object's xyz rotation

	MDR_UPDATE_OBJECT_POS_ALL_BIT	= 0b00001000,		//Update the object's xyz position
	MDR_UPDATE_OBJECT_SCL_ALL_BIT	= 0b00010000,		//Update the object's xyz scale

	MDR_UPDATE_OBJECT_GEOMETRY_BIT	= 0b00100000,		//Update the object's geometry

	MDR_UPDATE_OBJECT_ALL_BITS		= 0b00011111		//Update everything
};



struct MdrObject {
	std::string name;
	uint64_t ID;

	vec3 pos;
	dvec3 rot;
	dvec3 scl;

	ObjectGeometry geometry;
	ObjectPhysics physics;

	uint64 update = MDR_UPDATE_OBJECT_ALL_BITS;

	MdrObject(std::string _name = "Undefined", vec3 _pos = { 0,0,0 }, dvec3 _rot = { 0,0,0 }, dvec3 _scl = { 10,10,10 }) {
		name = _name;
		ID = LAST_OBJECT_ID++;
		pos = _pos;
		rot = _rot;
		scl = _scl;
	}

};




//Loads an .OBJ file into the object's geometry, overwriting it
//	object		a pointer to the MdrObject where to load the obj
//	objPath		the path to the .OBJ file
//	Returns true if the obj loading succeed. false if the file is not found or invalid
static bool mdrObjectLoadObj(MdrObject* object, std::string objPath) {
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
