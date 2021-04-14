
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#include "Lynx/shaders/Border2.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#define LNX_H_BORDER2



namespace lnx::shd{


	void Border2::create(vram::ptr<f32v4, VRam, Storage> pColorOutput, vram::ptr<u32v2, VRam, Storage> pWindowSize, vram::ptr<u32, VRam, Storage> pZBuffer, const u32v3 vGroupCount, Window& pWindow){
		pWindow.addObject_m.lock();
			colorOutput_.vdata = (vram::ptr<char, VRam, Storage>)pColorOutput;
			windowSize_.vdata = (vram::ptr<char, VRam, Storage>)pWindowSize;
			zBuffer_.vdata = (vram::ptr<char, VRam, Storage>)pZBuffer;

			createDescriptorSets();
			createCommandBuffers(vGroupCount, pWindow);
			pWindow.swp.shadersCBs.add(commandBuffers[0]);
		pWindow.addObject_m.unlock();
	}


	void Border2::createDescriptorSets(){
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
		switch(core::dvc::graphics.LD.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){
			case vk::Result::eErrorFragmentationEXT:  dbg::printError("Fragmentation error");  break;
			vkDefaultCases;
		}



		auto allocateSetInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool     (descriptorPool)
			.setDescriptorSetCount (1)
			.setPSetLayouts        (&Border2::layout.descriptorSetLayout)
		;
		switch(core::dvc::graphics.LD.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){
			case vk::Result::eErrorFragmentedPool:    dbg::printError("Fragmented pool");      break;
			case vk::Result::eErrorOutOfPoolMemory:   dbg::printError("Out of pool memory");   break;
			vkDefaultCases;
		}



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
		core::dvc::graphics.LD.updateDescriptorSets(4, writeSets, 0, nullptr);
	}








	void Border2::createCommandBuffers(const u32v3 vGroupCount, Window& pWindow){
		auto allocateCbInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (pWindow.commandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		commandBuffers.resize(1);
		switch(core::dvc::graphics.LD.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){ vkDefaultCases; }

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		commandBuffers[0].begin(beginInfo);
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[Border2::pipelineIndex]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, Border2::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
		commandBuffers[0].end();
	}








	void Border2::updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow){
		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		commandBuffers[0].begin(beginInfo);
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[Border2::pipelineIndex]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, Border2::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
		commandBuffers[0].end();
	}








	void Border2::destroy(){
		//TODO
	}








	Shader_b::Layout Border2::layout;
	uint32 Border2::pipelineIndex = core::shaders::pipelineNum++;
	LnxAutoInit(LNX_H_BORDER2){
		core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);
		core::shaders::pipelineLayouts[Border2::pipelineIndex] = &Border2::layout;
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
			switch(core::dvc::graphics.LD.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &Border2::layout.descriptorSetLayout)){ vkDefaultCases; }
		}




		{ //Create pipeline layout
			uint64 fileLength = 0;
			uint32* code = core::shaders::loadSpv(&fileLength, (core::shaders::shaderPath + "Border2.spv").begin());
			Border2::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.LD, code, fileLength);

			Border2::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
				.setStage  (vk::ShaderStageFlagBits::eCompute)
				.setModule (Border2::layout.shaderModule)
				.setPName  ("main")
			;

			auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount (1)
				.setPSetLayouts    (&Border2::layout.descriptorSetLayout)
			;
			switch(core::dvc::graphics.LD.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &Border2::layout.pipelineLayout)){ vkDefaultCases; }
		}
	}
}