
//####################################################################################
// This file was generated by Lynx/Tools/Build/GlslToCpp
// Changes could be overwritten without notice
//####################################################################################
#include "Generated/Shaders/FloatToIntBuffer.gsi.hpp"

#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Render.hpp"
#define LNX_H_FLOATTOINTBUFFER



namespace lnx::shd::gsi{


	void FloatToIntBuffer::spawn(
		const l_src& pSrc,
		const l_dst& pDst,
		const l_wsize& pWsize,
		const l_zbuff& pZbuff,
		const u32v3 vGroupCount, core::RenderCore& pRenderCore
	){
		pRenderCore.addObject_m.lock();
			src = pSrc;
			dst = pDst;
			wsize = pWsize;
			zbuff = pZbuff;

			createDescriptorSets();
			createCommandBuffers(vGroupCount, pRenderCore);
			pRenderCore.swp.shadersCBs.add(commandBuffers[0]);
		pRenderCore.addObject_m.unlock();
	}


	void FloatToIntBuffer::createDescriptorSets(){
		vk::DescriptorPoolSize sizes[2] = {
			vk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(4),{}
		};
		auto poolInfo = vk::DescriptorPoolCreateInfo()
			.setFlags         (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets       (1)
			.setPoolSizeCount (1)
			.setPPoolSizes    (sizes)
		;
		switch(core::dvc::graphics.ld.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){
			case vk::Result::eErrorFragmentationEXT:  dbg::logError("Fragmentation error");  break;
			vkDefaultCases;
		}



		auto allocateSetInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool     (descriptorPool)
			.setDescriptorSetCount (1)
			.setPSetLayouts        (&FloatToIntBuffer::layout.descriptorSetLayout)
		;
		switch(core::dvc::graphics.ld.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){
			case vk::Result::eErrorFragmentedPool:    dbg::logError("Fragmented pool");      break;
			case vk::Result::eErrorOutOfPoolMemory:   dbg::logError("Out of pool memory");   break;
			vkDefaultCases;
		}



		vk::WriteDescriptorSet writeSets[4];
		auto bufferInfo0 = vk::DescriptorBufferInfo()
			.setBuffer (src.vdata.cell->csc.buffer)
			.setOffset (src.vdata.cell->localOffset)
			.setRange  (src.vdata.cell->cellSize)
		;
		writeSets[0] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (0)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo0)
		;

		auto bufferInfo1 = vk::DescriptorBufferInfo()
			.setBuffer (dst.vdata.cell->csc.buffer)
			.setOffset (dst.vdata.cell->localOffset)
			.setRange  (dst.vdata.cell->cellSize)
		;
		writeSets[1] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (1)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo1)
		;

		auto bufferInfo2 = vk::DescriptorBufferInfo()
			.setBuffer (wsize.vdata.cell->csc.buffer)
			.setOffset (wsize.vdata.cell->localOffset)
			.setRange  (wsize.vdata.cell->cellSize)
		;
		writeSets[2] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (2)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo2)
		;

		auto bufferInfo3 = vk::DescriptorBufferInfo()
			.setBuffer (zbuff.vdata.cell->csc.buffer)
			.setOffset (zbuff.vdata.cell->localOffset)
			.setRange  (zbuff.vdata.cell->cellSize)
		;
		writeSets[3] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (3)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo3)
		;
		core::dvc::graphics.ld.updateDescriptorSets(4, writeSets, 0, nullptr);
	}








void FloatToIntBuffer::createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){
	auto allocateCbInfo = vk::CommandBufferAllocateInfo()
		.setCommandPool        (pRenderCore.commandPool)
		.setLevel              (vk::CommandBufferLevel::ePrimary)
		.setCommandBufferCount (1)
	;
	commandBuffers.resize(1);
	switch(core::dvc::graphics.ld.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){ vkDefaultCases; }

	auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
	switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }
	commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[FloatToIntBuffer::pipelineIndex]);
	commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, FloatToIntBuffer::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
	switch(commandBuffers[0].end()){ vkDefaultCases; }
}








void FloatToIntBuffer::updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){
	auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
	switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }
	commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[FloatToIntBuffer::pipelineIndex]);
	commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, FloatToIntBuffer::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
	switch(commandBuffers[0].end()){ vkDefaultCases; }
}








void FloatToIntBuffer::destroy(){
	//TODO
}








ShaderInterface_b::Layout FloatToIntBuffer::layout;
uint32 FloatToIntBuffer::pipelineIndex = core::shaders::pipelineNum++;
LnxAutoInit(LNX_H_FLOATTOINTBUFFER){
	core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);
	core::shaders::pipelineLayouts[FloatToIntBuffer::pipelineIndex] = &FloatToIntBuffer::layout;
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
			.setDescriptorType     (vk::DescriptorType::eStorageBuffer)
			.setDescriptorCount    (1)
			.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
			.setPImmutableSamplers (nullptr)
		;

		auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount (4)
			.setPBindings    (bindingLayouts)
		;
		//Create the descriptor set layout
		switch(core::dvc::graphics.ld.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &FloatToIntBuffer::layout.descriptorSetLayout)){ vkDefaultCases; }
	}




	{ //Create pipeline layout
		uint64 fileLength = 0;
		uint32* code = core::shaders::loadSpv(&fileLength, "src/Generated/FloatToIntBuffer.spv");
		FloatToIntBuffer::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.ld, code, fileLength);

		FloatToIntBuffer::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage  (vk::ShaderStageFlagBits::eCompute)
			.setModule (FloatToIntBuffer::layout.shaderModule)
			.setPName  ("main")
		;

		auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount (1)
			.setPSetLayouts    (&FloatToIntBuffer::layout.descriptorSetLayout)
		;
		switch(core::dvc::graphics.ld.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &FloatToIntBuffer::layout.pipelineLayout)){ vkDefaultCases; }
	}
}
}