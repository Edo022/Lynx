
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#include "LuxEngine/shaders/Border2.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#define LUX_H_BORDER2



namespace lux::shd{


	void Border2::createDescriptorSets(){ //FIXME REMOVE LAYOUT
		vk::DescriptorPoolSize sizes[2] = {
			vk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(3),
			vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(1)
		};
		auto poolInfo = vk::DescriptorPoolCreateInfo()
			.setFlags         (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets       (1)
			.setPoolSizeCount (2)
			.setPPoolSizes    (sizes)
		;
		core::dvc::compute.LD.createDescriptorPool(&poolInfo, nullptr, &descriptorPool); //FIXME CHECK RETURN



		auto allocateSetInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool     (descriptorPool)
			.setDescriptorSetCount (1)
			.setPSetLayouts        (&Border2::layout.descriptorSetLayout)
		;
		core::dvc::compute.LD.allocateDescriptorSets(&allocateSetInfo, &descriptorSet);



		vk::WriteDescriptorSet writeSets[4];
		auto bufferInfo0 = vk::DescriptorBufferInfo()
			.setBuffer (colorOutput_.vdata.cell->csc.buffer)
			.setOffset (colorOutput_.vdata.cell->localOffset)
			.setRange  (colorOutput_.vdata.cell->cellSize)
		;
		writeSets[0] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (0)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo0)
		;

		auto bufferInfo1 = vk::DescriptorBufferInfo()
			.setBuffer (windowSize_.vdata.cell->csc.buffer)
			.setOffset (windowSize_.vdata.cell->localOffset)
			.setRange  (windowSize_.vdata.cell->cellSize)
		;
		writeSets[1] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (1)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo1)
		;

		auto bufferInfo2 = vk::DescriptorBufferInfo()
			.setBuffer (zBuffer_.vdata.cell->csc.buffer)
			.setOffset (zBuffer_.vdata.cell->localOffset)
			.setRange  (zBuffer_.vdata.cell->cellSize)
		;
		writeSets[2] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (2)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo2)
		;

		auto bufferInfo3 = vk::DescriptorBufferInfo()
			.setBuffer (objData_.vdata.cell->csc.buffer)
			.setOffset (objData_.vdata.cell->localOffset)
			.setRange  (objData_.vdata.cell->cellSize)
		;
		writeSets[3] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (3)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eUniformBuffer)
			.setPBufferInfo     (&bufferInfo3)
		;
		core::dvc::compute.LD.updateDescriptorSets(4, writeSets, 0, nullptr);
	}








	void Border2::createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){ //FIXME REMOVE LAYOUT
		auto allocateCbInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (pWindow.commandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		commandBuffers.resize(1);
		core::dvc::compute.LD.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin());

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		commandBuffers[0].begin(beginInfo);
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[vShaderLayout]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, Border2::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCountX, vGroupCountY, vGroupCountZ);
		commandBuffers[0].end();
	}








	void Border2::updateCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){
		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		commandBuffers[0].begin(beginInfo);
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[vShaderLayout]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, Border2::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCountX, vGroupCountY, vGroupCountZ);
		commandBuffers[0].end();
	}








	void Border2::destroy(){
		//TODO
	}








	Shader_b::Layout Border2::layout;
	luxAutoInit(LUX_H_BORDER2){
		{ //Create descriptor set layout
			vk::DescriptorSetLayoutBinding bindingLayouts[4];
			bindingLayouts[0] = vk::DescriptorSetLayoutBinding()
				.setBinding            (0)
				.setDescriptorType     (vk::DescriptorType::eStorageBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;

			bindingLayouts[1] = vk::DescriptorSetLayoutBinding()
				.setBinding            (1)
				.setDescriptorType     (vk::DescriptorType::eStorageBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;

			bindingLayouts[2] = vk::DescriptorSetLayoutBinding()
				.setBinding            (2)
				.setDescriptorType     (vk::DescriptorType::eStorageBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;

			bindingLayouts[3] = vk::DescriptorSetLayoutBinding()
				.setBinding            (3)
				.setDescriptorType     (vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;

			auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
				.setBindingCount (4)
				.setPBindings    (bindingLayouts)
			;
			//Create the descriptor set layout
			core::dvc::compute.LD.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &Border2::layout.descriptorSetLayout);
		}




		{ //Create pipeline layout
			uint32 fileLength;
			Border2::layout.shaderModule = core::c::shaders::cshaderCreateModule(
				core::dvc::compute.LD, core::c::shaders::cshaderReadFromFile(
					&fileLength,
					(core::c::shaders::shaderPath + "Border2.spv").begin()
				),
				&fileLength
			);

			Border2::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
				.setStage  (vk::ShaderStageFlagBits::eCompute)
				.setModule (Border2::layout.shaderModule)
				.setPName  ("main")
			;

			auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount (1)
				.setPSetLayouts    (&Border2::layout.descriptorSetLayout)
			;
			core::dvc::compute.LD.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &Border2::layout.pipelineLayout);
		}
	}
}