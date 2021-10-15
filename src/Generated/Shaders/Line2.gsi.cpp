
//####################################################################################
// This file was generated by Tools/Build/Generators/GenInterfaces
// Changes could be overwritten without notice
//####################################################################################
#include "Generated/Shaders/Line2.gsi.hpp"

#include "Lynx/Core/Init.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Render.hpp"



namespace lnx::shd::gsi{


	void Line2::spawn(
		const l_outcol& pOutcol,
		const l_wsize& pWsize,
		const l_zbuff& pZbuff,
		const u32v3 vGroupCount, core::RenderCore& pRenderCore
	){
		pRenderCore.addObject_m.lock();
			outcol = pOutcol;
			outcol._pvt_elm_outcol = (f32v4*)(outcol.data + 0);
			wsize = pWsize;
			wsize._pvt_elm_wsize = (u32v2*)(wsize.data + 0);
			zbuff = pZbuff;
			zbuff._pvt_elm_zbuff = (u32*)(zbuff.data + 0);

			createDescriptorSets();
			createCommandBuffers(vGroupCount, pRenderCore);
			pRenderCore.swp.shadersCBs.add(commandBuffers[0]);
		pRenderCore.addObject_m.unlock();
	}








	void Line2::createDescriptorSets(){
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
		switch(core::dvc::g_graphics().ld.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){
			case vk::Result::eErrorFragmentationEXT:  dbg::logError("Fragmentation error");  break;
			vkDefaultCases;
		}



		auto allocateSetInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool     (descriptorPool)
			.setDescriptorSetCount (1)
			.setPSetLayouts        (&g_Line2_layout().descriptorSetLayout)
		;
		switch(core::dvc::g_graphics().ld.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){
			case vk::Result::eErrorFragmentedPool:    dbg::logError("Fragmented pool");      break;
			case vk::Result::eErrorOutOfPoolMemory:   dbg::logError("Out of pool memory");   break;
			vkDefaultCases;
		}



		vk::WriteDescriptorSet writeSets[4];

		auto bufferInfo0 = vk::DescriptorBufferInfo()
			.setBuffer (data.vdata.cell->csc.buffer)
			.setOffset (data.vdata.cell->localOffset)
			.setRange  (data.vdata.cell->cellSize)
		;
		writeSets[0] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (3)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eUniformBuffer)
			.setPBufferInfo     (&bufferInfo0)
		;

		auto bufferInfo1 = vk::DescriptorBufferInfo()
			.setBuffer (outcol.vdata.cell->csc.buffer)
			.setOffset (outcol.vdata.cell->localOffset)
			.setRange  (outcol.vdata.cell->cellSize)
		;
		writeSets[1] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (0)
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
			.setDstBinding      (1)
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
			.setDstBinding      (2)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo3)
		;
		core::dvc::g_graphics().ld.updateDescriptorSets(4, writeSets, 0, nullptr);
	}








	void Line2::createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){
		auto allocateCbInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (pRenderCore.commandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		commandBuffers.resize(1);
		switch(core::dvc::g_graphics().ld.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){ vkDefaultCases; }

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[g_Line2_pipelineIndex()]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, g_Line2_layout().pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
		switch(commandBuffers[0].end()){ vkDefaultCases; }
	}








	void Line2::updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){
		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[g_Line2_pipelineIndex()]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, g_Line2_layout().pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);
		switch(commandBuffers[0].end()){ vkDefaultCases; }
	}








	void Line2::destroy(){
		//TODO
	}








	_lnx_init_var_value_def((InterfaceLayout), Line2_layout,        lnx::shd::gsi){}
	_lnx_init_var_value_def((uint32),          Line2_pipelineIndex, lnx::shd::gsi){ *pVar = core::shaders::g_pipelineNum()++; }
	_lnx_init_fun_def(LNX_H_LINE2, lnx::shd::gsi){
		core::shaders::g_pipelineLayouts().resize(core::shaders::g_pipelineNum());
		core::shaders::g_pipelineLayouts()[g_Line2_pipelineIndex()] = &g_Line2_layout();
		{ //Create descriptor set layout
			vk::DescriptorSetLayoutBinding bindingLayouts[4];


			bindingLayouts[0] = vk::DescriptorSetLayoutBinding()
				.setBinding            (3)
				.setDescriptorType     (vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;


			bindingLayouts[1] = vk::DescriptorSetLayoutBinding()
				.setBinding            (0)
				.setDescriptorType     (vk::DescriptorType::eStorageBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;


			bindingLayouts[2] = vk::DescriptorSetLayoutBinding()
				.setBinding            (1)
				.setDescriptorType     (vk::DescriptorType::eStorageBuffer)
				.setDescriptorCount    (1)
				.setStageFlags         (vk::ShaderStageFlagBits::eCompute)
				.setPImmutableSamplers (nullptr)
			;


			bindingLayouts[3] = vk::DescriptorSetLayoutBinding()
				.setBinding            (2)
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
			switch(core::dvc::g_graphics().ld.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &g_Line2_layout().descriptorSetLayout)){ vkDefaultCases; }
		}




		{ //Create pipeline layout
			uint64 fileLength = 0;
			uint32* code = core::shaders::loadSpv(&fileLength, "Lynx/src/Generated/Shaders/Line2.spv");
			g_Line2_layout().shaderModule = core::shaders::createModule(core::dvc::g_graphics().ld, code, fileLength);

			g_Line2_layout().shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
				.setStage  (vk::ShaderStageFlagBits::eCompute)
				.setModule (g_Line2_layout().shaderModule)
				.setPName  ("main")
			;

			auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount (1)
				.setPSetLayouts    (&g_Line2_layout().descriptorSetLayout)
			;
			switch(core::dvc::g_graphics().ld.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &g_Line2_layout().pipelineLayout)){ vkDefaultCases; }
		}
	}
}