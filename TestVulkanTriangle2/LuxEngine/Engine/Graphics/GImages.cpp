
#include "LuxEngine/Engine/Engine.h"









// Geometry ---------------------------------------------------------------------------------------------------------------------------------//








void Engine::createVertexBuffer() {
	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(graphics.LD, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Map and copy memory
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




void Engine::createIndexBuffer() {
	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(graphics.LD, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Map and copy memory
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








// Textures ---------------------------------------------------------------------------------------------------------------------------------//








void Engine::createTextureImage() {
	createImage(WIDTH, HEIGHT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	if(CBuffers.isValid(0))copyBufferToImage(CBuffers[0].buffer, textureImage, WIDTH, HEIGHT);//TODO fix
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}




void Engine::createTextureImageView() {
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}




void Engine::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};							//Create sampler infos
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_BEGIN_RANGE;					//VK_FILTER_LINEAR;  begin range removes filtering
	samplerInfo.minFilter = VK_FILTER_BEGIN_RANGE;					//VK_FILTER_LINEAR;  begin range removes filtering

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture

	samplerInfo.anisotropyEnable = VK_FALSE;						//No need to enable the anistropy filter since we neither use meshes or textures
	samplerInfo.maxAnisotropy = 16;									//Maximum anistropy. Default at 16
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;		//Black background when the texture ends
	samplerInfo.unnormalizedCoordinates = VK_FALSE;					//Use normalized coordinates

	samplerInfo.compareEnable = VK_FALSE;							//Don't compare. We just need to put some pixels on the screen
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;					//(Useless)

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_BEGIN_RANGE;	//VK_SAMPLER_MIPMAP_MODE_LINEAR;  begin range doesn't use mipmap
	samplerInfo.mipLodBias = 0.0f;									//?? idk sorry
	samplerInfo.minLod = 0.0f;										//Minimum level of detail
	samplerInfo.maxLod = 0.0f;										//Maximum level of detail

	//Create sampler, exit if an error occurs. Sampler is needed for the render
	Try(vkCreateSampler(graphics.LD, &samplerInfo, nullptr, &textureSampler)) Quit("Failed to create texture sampler");
}








// Images -----------------------------------------------------------------------------------------------------------------------------------//








void Engine::createImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Try(vkCreateImage(graphics.LD, &imageInfo, nullptr, &image)) Quit("Failed to create image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(graphics.LD, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	Try(vkAllocateMemory(graphics.LD, &allocInfo, nullptr, &imageMemory)) Quit("Failed to allocate image memory");
	vkBindImageMemory(graphics.LD, image, imageMemory, 0);
}




VkImageView Engine::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView = VK_NULL_HANDLE;
	Try(vkCreateImageView(graphics.LD, &viewInfo, nullptr, &imageView)) Quit("Failed to create texture image view");
	return imageView;
}




void Engine::copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	endSingleTimeCommands(commandBuffer);
}




void Engine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage, destinationStage;
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else Quit("Unsupported layout transition");

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	endSingleTimeCommands(commandBuffer);
}
