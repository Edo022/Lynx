#pragma once

#include <vulkan/vulkan.h>
#include "LuxEngine/Types/Containers/LuxDynArray.h"
#include "LuxEngine/Types/Vectors/Vectors.h"
#pragma warning( disable : 26812 )	//Prefer enum class to enum



//Vertices used in rendering
struct Vertex {
	fvec3 pos;
	fvec3 color;
	fvec2 texCoord;

	inline static VkVertexInputBindingDescription getBindingDescription() {
		return VkVertexInputBindingDescription{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
	}

	static LuxDynArray<VkVertexInputAttributeDescription> getAttributeDescriptions() {
		LuxDynArray<VkVertexInputAttributeDescription> attributeDescriptions(3, 3);
		attributeDescriptions.add(VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT,	offsetof(Vertex, pos) });
		attributeDescriptions.add(VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT,	offsetof(Vertex, color) });
		attributeDescriptions.add(VkVertexInputAttributeDescription{ 2, 0, VK_FORMAT_R32G32_SFLOAT,		offsetof(Vertex, texCoord) });
		return attributeDescriptions;
	}
};
