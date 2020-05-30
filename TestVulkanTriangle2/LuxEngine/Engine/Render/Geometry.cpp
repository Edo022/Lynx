
#include "LuxEngine/Engine/Engine.h"



void Render::createVertexBuffer() {
	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(graphics.LD, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Map memory
	void* data;
	vkMapMemory(graphics.LD, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (int64)bufferSize);
	vkUnmapMemory(graphics.LD, stagingBufferMemory);

	//Create vertex buffer
	createBuffer(graphics.LD, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	//Destroy staging buffer
	vkDestroyBuffer(graphics.LD, stagingBuffer, nullptr);
	vkFreeMemory(graphics.LD, stagingBufferMemory, nullptr);
}




void Render::createIndexBuffer() {
	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(graphics.LD, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Map memory
	void* data;
	vkMapMemory(graphics.LD, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (int64)bufferSize);
	vkUnmapMemory(graphics.LD, stagingBufferMemory);

	//Create index buffer
	createBuffer(graphics.LD, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	//Destroy staging buffer
	vkDestroyBuffer(graphics.LD, stagingBuffer, nullptr);
	vkFreeMemory(graphics.LD, stagingBufferMemory, nullptr);
}
