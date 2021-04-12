
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#include "Lynx/shaders/FloatToIntBuffer.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#define LNX_H_FLOATTOINTBUFFER



namespace lnx::shd{


	void FloatToIntBuffer::createDescriptorSets(){ //FIXME REMOVE LAYOUT
		vk::DescriptorPoolSize sizes[2] = {
			vk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(4),
			{}
		};
		auto poolInfo = vk::DescriptorPoolCreateInfo()
			.setFlags         (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets       (1)
			.setPoolSizeCount (1)
			.setPPoolSizes    (sizes)
		;
		switch(core::dvc::compute.LD.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){
			case vk::Result::eErrorFragmentationEXT:  dbg::printError("Fragmentation error");  break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
			case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		}



		auto allocateSetInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool     (descriptorPool)
			.setDescriptorSetCount (1)
			.setPSetLayouts        (&FloatToIntBuffer::layout.descriptorSetLayout)
		;
		switch(core::dvc::compute.LD.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){
			case vk::Result::eErrorFragmentedPool:    dbg::printError("Fragmented pool");      break;
			case vk::Result::eErrorOutOfPoolMemory:   dbg::printError("Out of pool memory");   break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
			case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		}



		vk::WriteDescriptorSet writeSets[4];
		auto bufferInfo0 = vk::DescriptorBufferInfo()
			.setBuffer (src_.vdata.cell->csc.buffer)
			.setOffset (src_.vdata.cell->localOffset)
			.setRange  (src_.vdata.cell->cellSize)
		;
		writeSets[0] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (0)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo0)
		;

		auto bufferInfo1 = vk::DescriptorBufferInfo()
			.setBuffer (dst_.vdata.cell->csc.buffer)
			.setOffset (dst_.vdata.cell->localOffset)
			.setRange  (dst_.vdata.cell->cellSize)
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
			.setBuffer (windowSize_.vdata.cell->csc.buffer)
			.setOffset (windowSize_.vdata.cell->localOffset)
			.setRange  (windowSize_.vdata.cell->cellSize)
		;
		writeSets[3] = vk::WriteDescriptorSet()
			.setDstSet          (descriptorSet)
			.setDstBinding      (3)
			.setDescriptorCount (1)
			.setDescriptorType  (vk::DescriptorType::eStorageBuffer)
			.setPBufferInfo     (&bufferInfo3)
		;
		core::dvc::compute.LD.updateDescriptorSets(4, writeSets, 0, nullptr);
	}








	void FloatToIntBuffer::createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){ //FIXME REMOVE LAYOUT
		auto allocateCbInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (pWindow.commandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		commandBuffers.resize(1);
		switch(core::dvc::compute.LD.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
			case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		}

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		commandBuffers[0].begin(beginInfo);
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[vShaderLayout]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, FloatToIntBuffer::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCountX, vGroupCountY, vGroupCountZ);
		commandBuffers[0].end();
	}








	void FloatToIntBuffer::updateCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){
		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		commandBuffers[0].begin(beginInfo);
		commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[vShaderLayout]);
		commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, FloatToIntBuffer::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		commandBuffers[0].dispatch           (vGroupCountX, vGroupCountY, vGroupCountZ);
		commandBuffers[0].end();
	}








	void FloatToIntBuffer::destroy(){
		//TODO
	}








	Shader_b::Layout FloatToIntBuffer::layout;
	LnxAutoInit(LNX_H_FLOATTOINTBUFFER){
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
			switch(core::dvc::compute.LD.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &FloatToIntBuffer::layout.descriptorSetLayout)){
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
				case vk::Result::eSuccess: break;
				default: dbg::printError("Unknown result");
			}
		}




		{ //Create pipeline layout
			uint64 fileLength = 0;
			uint32* code = core::c::shaders::loadSpv(&fileLength, (core::c::shaders::shaderPath + "FloatToIntBuffer.spv").begin());
			FloatToIntBuffer::layout.shaderModule = core::c::shaders::createModule(core::dvc::compute.LD, code, fileLength);

			FloatToIntBuffer::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
				.setStage  (vk::ShaderStageFlagBits::eCompute)
				.setModule (FloatToIntBuffer::layout.shaderModule)
				.setPName  ("main")
			;

			auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount (1)
				.setPSetLayouts    (&FloatToIntBuffer::layout.descriptorSetLayout)
			;
			switch(core::dvc::compute.LD.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &FloatToIntBuffer::layout.pipelineLayout)){
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
				case vk::Result::eSuccess: break;
				default: dbg::printError("Unknown result");
			}
		}
	}
}