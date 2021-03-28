
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct FloatToIntBuffer : public Shader_b {


		struct src__t : public ShaderElm_b<Storage> {
			src__t() {
				ShaderElm_b::bind = 0;
			}
		} src_;


		struct dst__t : public ShaderElm_b<Storage> {
			dst__t() {
				ShaderElm_b::bind = 1;
			}
		} dst_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
		} zBuffer_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::bind = 3;
			}
		} windowSize_;


		void create(vram::ptr<f32v4, VRam, Storage> pSrc, vram::ptr<u32, VRam, Storage> pDst, vram::ptr<u32, VRam, Storage> pZBuffer, vram::ptr<u32, VRam, Storage> pWidth){
			src_.vdata = (vram::ptr<char, VRam, Storage>)pSrc;
			dst_.vdata = (vram::ptr<char, VRam, Storage>)pDst;
			zBuffer_.vdata = (vram::ptr<char, VRam, Storage>)pZBuffer;
			windowSize_.vdata = (vram::ptr<char, VRam, Storage>)pWidth;
		}


		void createDescriptorSets(const ShaderLayout vShaderLayout, Window& pWindow){ //FIXME REMOVE LAYOUT
			VkDescriptorPoolSize sizes[2] = {
				{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 4 },
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



			VkWriteDescriptorSet writeSets[4];
			VkDescriptorBufferInfo bufferInfo0 = {
				.buffer = src_.vdata.cell->csc.buffer,
				.offset = src_.vdata.cell->localOffset,
				.range  = src_.vdata.cell->cellSize
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
				.buffer = dst_.vdata.cell->csc.buffer,
				.offset = dst_.vdata.cell->localOffset,
				.range  = dst_.vdata.cell->cellSize
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
				.buffer = windowSize_.vdata.cell->csc.buffer,
				.offset = windowSize_.vdata.cell->localOffset,
				.range  = windowSize_.vdata.cell->cellSize
			};
			writeSets[3] = VkWriteDescriptorSet{
				.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet          = descriptorSet,
				.dstBinding      = 3,
				.descriptorCount = 1,
				.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pBufferInfo     = &bufferInfo3
			};
			vkUpdateDescriptorSets(core::dvc::compute.LD, 4, writeSets, 0, nullptr);
		}


		void createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){ //FIXME REMOVE LAYOUT
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
	};
}//TODO remove local data in external bindings