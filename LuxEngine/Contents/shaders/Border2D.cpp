
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#include "LuxEngine/Contents/shaders/Border2D.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"



namespace lux::shd{


	void Border2D::createDescriptorSets(const ShaderLayout vShaderLayout, Window& pWindow){ //FIXME REMOVE LAYOUT
		VkDescriptorPoolSize sizes[2] = {
			{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 3 },
			{}
		};
		VkDescriptorPoolCreateInfo poolInfo = {
			.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets       = 1,
			.poolSizeCount = 1,
			.pPoolSizes    = sizes
		};
		vkCreateDescriptorPool(core::dvc::compute.LD, &poolInfo, nullptr, &descriptorPool); //FIXME CHECK RETURN



		VkDescriptorSetAllocateInfo allocateSetInfo = {
			.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool     = descriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts        = &pWindow.CShadersLayouts[vShaderLayout].descriptorSetLayout
		};
		vkAllocateDescriptorSets(core::dvc::compute.LD, &allocateSetInfo, &descriptorSet);



		VkWriteDescriptorSet writeSets[3];
		VkDescriptorBufferInfo bufferInfo0 = {
			.buffer = colorOutput_.vdata.cell->csc.buffer,
			.offset = colorOutput_.vdata.cell->localOffset,
			.range  = colorOutput_.vdata.cell->cellSize
		};
		writeSets[0] = VkWriteDescriptorSet{
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = descriptorSet,
			.dstBinding      = 0,
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pBufferInfo     = &bufferInfo0
		};

		VkDescriptorBufferInfo bufferInfo1 = {
			.buffer = windowSize_.vdata.cell->csc.buffer,
			.offset = windowSize_.vdata.cell->localOffset,
			.range  = windowSize_.vdata.cell->cellSize
		};
		writeSets[1] = VkWriteDescriptorSet{
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = descriptorSet,
			.dstBinding      = 1,
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pBufferInfo     = &bufferInfo1
		};

		VkDescriptorBufferInfo bufferInfo2 = {
			.buffer = zBuffer_.vdata.cell->csc.buffer,
			.offset = zBuffer_.vdata.cell->localOffset,
			.range  = zBuffer_.vdata.cell->cellSize
		};
		writeSets[2] = VkWriteDescriptorSet{
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = descriptorSet,
			.dstBinding      = 2,
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pBufferInfo     = &bufferInfo2
		};

		VkDescriptorBufferInfo bufferInfo3 = {
			.buffer = objData_.vdata.cell->csc.buffer,
			.offset = objData_.vdata.cell->localOffset,
			.range  = objData_.vdata.cell->cellSize
		};
		writeSets[3] = VkWriteDescriptorSet{
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = descriptorSet,
			.dstBinding      = 3,
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo     = &bufferInfo3
		};
		vkUpdateDescriptorSets(core::dvc::compute.LD, 3, writeSets, 0, nullptr);
	}








	void Border2D::createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){ //FIXME REMOVE LAYOUT
		VkCommandBufferAllocateInfo allocateCbInfo = {
			.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool        = pWindow.commandPool,
			.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		commandBuffers.resize(1);
		vkAllocateCommandBuffers(core::dvc::compute.LD, &allocateCbInfo, commandBuffers.begin());

		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
		};
		vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

		vkCmdBindPipeline      (commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipeline);
		vkCmdBindDescriptorSets(commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdDispatch          (commandBuffers[0], vGroupCountX, vGroupCountY, vGroupCountZ);

		vkEndCommandBuffer(commandBuffers[0]);
	}








	void Border2D::updateCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
		};
		vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

		vkCmdBindPipeline      (commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipeline);
		vkCmdBindDescriptorSets(commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdDispatch          (commandBuffers[0], vGroupCountX, vGroupCountY, vGroupCountZ);

		vkEndCommandBuffer(commandBuffers[0]);
	}
}