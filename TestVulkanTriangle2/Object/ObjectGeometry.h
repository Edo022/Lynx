#pragma once


#include "RenderEngine/Structs/Vertex.h"
#include "Types/Containers/LuxArray.h"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>
#include <tiny_obj_loader.h>
#include <string>



struct ObjectGeometry {
	//Mesh data
	LuxArray<Vertex> vertices;	//Vertices of the mesh
	LuxArray<uint32> indices;	//Triangles of the mesh. 3 indices per tiangle //uint??_t LLID0

	//GPU buffers. Those are private variables, you shouldn't use them.
	VkBuffer __vertexBuffer;
	VkDeviceMemory __vertexBufferMemory;
	VkBuffer __indexBuffer;
	VkDeviceMemory __indexBufferMemory;
	std::vector<VkBuffer> __uniformBuffers; //TODO use luxARray
	std::vector<VkDeviceMemory> __uniformBuffersMemory;
	glm::mat4 model;
	//UniformBufferObject ubo{};

	ObjectGeometry(LuxArray<Vertex> _vertices = {}, LuxArray<uint32> _indices = {}) {
		vertices = _vertices;
		indices = _indices;

		//TODO fill buffer
	}
};


