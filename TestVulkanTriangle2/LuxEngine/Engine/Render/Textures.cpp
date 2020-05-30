
#include "LuxEngine/Engine/Engine.h"







void Render::createTextureImage() {
	runCommandBuffer();
	createImage(WIDTH, HEIGHT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(buffer, textureImage, WIDTH, HEIGHT);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}




void Render::createTextureImageView() {
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}




void Render::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};								//Sampler infos
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_BEGIN_RANGE;					//VK_FILTER_LINEAR;  begin range removes filtering
	samplerInfo.minFilter = VK_FILTER_BEGIN_RANGE;					//VK_FILTER_LINEAR;  begin range removes filtering

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture

	samplerInfo.anisotropyEnable = VK_FALSE;						//VK_TRUE;  no need to enable the anistropy filter since we neither use meshes or textures
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
