
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#include "Lynx/shaders/3DTest.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#define LNX_H__3DTEST



namespace lnx::shd{


	void _3DTest::create(vram::ptr<f32v4, VRam, Storage> pOutcol, vram::ptr<u32v2, VRam, Storage> pWsize, vram::ptr<f32v4, VRam, Uniform> pData, const u32v3 vGroupCount, Window& pWindow){
		pWindow.addObject_m.lock();
			_outcol.vdata = (vram::ptr<char, VRam, Storage>)pOutcol;
			_wsize.vdata = (vram::ptr<char, VRam, Storage>)pWsize;
			_data.vdata = (vram::ptr<char, VRam, Uniform>)pData;

			createDescriptorSets();
			createCommandBuffers(vGroupCount, pWindow);
			pWindow.swp.shadersCBs.add(commandBuffers[0]);
		pWindow.addObject_m.unlock();
	}


	void _3DTest::createDescriptorSets(){
		vk::DescriptorPoolSize sizes[2] = {
			vk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(2),
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
			.setPSetLayouts        (&_3DTest::layout.descriptorSetLayout)
		;
		switch(core::dvc::graphics.LD.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){
			case vk::Result::eErrorFragmentedPool:    dbg::printError("Fragmented pool");      break;
			case vk::Result::eErrorOutOfPoolMemory:   dbg::printError("Out of pool memory");   break;
			vkDefaultCases;
		}



		vk::WriteDescriptorSet writeSets[3];
		auto bufferInfo0 = vk::DescriptorBufferInfo()
			.setBuffer (_outcol.vdata.cell->csc.buffer)
			.setOffset (_outcol.vdata.cell->localOffset)
			.setRange  (_outcol.vdata.cell->cellSize)
		;
		writeSets[0] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (0)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo0)
		;

		auto bufferInfo1 = vk::DescriptorBufferInfo()
			.setBuffer (_wsize.vdata.cell->csc.buffer)
			.setOffset (_wsize.vdata.cell->localOffset)
			.setRange  (_wsize.vdata.cell->cellSize)
		;
		writeSets[1] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (1)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo1)
		;

		auto bufferInfo2 = vk::DescriptorBufferInfo()
			.setBuffer (_data.vdata.cell->csc.buffer)
			.setOffset (_data.vdata.cell->localOffset)
			.setRange  (_data.vdata.cell->cellSize)
		;
		writeSets[2] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (3)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eUniformBuffer)
			.setPBufferInfo     (&bufferInfo2)
		;
		core::dvc::graphics.LD.updateDescriptorSets(3, writeSets, 0, nullptr);
	}








	void _3DTest::createCommandBuffers(const u32v3 vGroupCount, Window& pWindow){
		auto allocateCbInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (pWindow.commandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		commandBuffers.resize(1);
		switch(core::dvc::graphics.LD.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){ vkDefaultCases; }

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[_3DTest::pipelineIndex]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, _3DTest::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
		switch(commandBuffers[0].end()){ vkDefaultCases; }
	}








	void _3DTest::updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow){
		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[_3DTest::pipelineIndex]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, _3DTest::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
		switch(commandBuffers[0].end()){ vkDefaultCases; }
	}








	void _3DTest::destroy(){
		//TODO
	}








	Shader_b::Layout _3DTest::layout;
	uint32 _3DTest::pipelineIndex = core::shaders::pipelineNum++;
	LnxAutoInit(LNX_H__3DTEST){
		core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);
		core::shaders::pipelineLayouts[_3DTest::pipelineIndex] = &_3DTest::layout;
		{ //Create descriptor set layout
			vk::DescriptorSetLayoutBinding bindingLayouts[3];
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
				.setBinding            (3)
				.setDescriptorType     (vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;

			auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
				.setBindingCount (3)
				.setPBindings    (bindingLayouts)
			;
			//Create the descriptor set layout
			switch(core::dvc::graphics.LD.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &_3DTest::layout.descriptorSetLayout)){ vkDefaultCases; }
		}




		{ //Create pipeline layout
			uint64 fileLength = 0;
			uint32* code = core::shaders::loadSpv(&fileLength, (core::shaders::shaderPath + "_3DTest.spv").begin());
			_3DTest::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.LD, code, fileLength);

			_3DTest::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
				.setStage  (vk::ShaderStageFlagBits::eCompute)
				.setModule (_3DTest::layout.shaderModule)
				.setPName  ("main")
			;

			auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount (1)
				.setPSetLayouts    (&_3DTest::layout.descriptorSetLayout)
			;
			switch(core::dvc::graphics.LD.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &_3DTest::layout.pipelineLayout)){ vkDefaultCases; }
		}
	}
}