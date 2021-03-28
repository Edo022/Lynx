
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct Border2D : public Shader_b {


		struct colorOutput__t : public ShaderElm_b<Storage> {
			colorOutput__t() {
				ShaderElm_b::bind = 0;
			}
		} colorOutput_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::bind = 1;
			}
		} windowSize_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
		} zBuffer_;


		struct objData__t : public ShaderElm_b<Uniform> {
			objData__t() {
				ShaderElm_b::vdata.realloc(32);
				ShaderElm_b::data.realloc(32);
				ShaderElm_b::bind = 3;
			}
			//Position of the top-left corner
			alwaysInline f32v2& ffp() { return *(f32v2*)ShaderElm_b::data; }
			//Position of the bottom-right corner
			alwaysInline f32v2& fsp() { return *(f32v2*)(ShaderElm_b::data + 8); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(ShaderElm_b::data + 16); }
		} objData_;


		void create(vram::ptr<f32v4, VRam, Storage> pColorOutput, vram::ptr<u32v2, VRam, Storage> pWindow, vram::ptr<u32, VRam, Storage> pZBuffer){
			colorOutput_.vdata = (vram::ptr<char, VRam, Storage>)pColorOutput;
			windowSize_.vdata = (vram::ptr<char, VRam, Storage>)pWindow;
			zBuffer_.vdata = (vram::ptr<char, VRam, Storage>)pZBuffer;
		}


		void createDescriptorSets(const ShaderLayout vShaderLayout , Window& pWindow){ //FIXME REMOVE LAYOUT
			VkDescriptorPoolSize sizes[2] = {
				{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 3 },
				{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1 }
			};
			VkDescriptorPoolCreateInfo poolInfo = {
				.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
				.maxSets       = 1,
				.poolSizeCount = 2,
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
			vkUpdateDescriptorSets(core::dvc::compute.LD, 4, writeSets, 0, nullptr);
		}
	};
}//TODO remove local data in external bindings