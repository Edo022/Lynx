
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#include "LuxEngine/LuxEngine/Contents/shaders/FloatToIntBuffer.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"



namespace lux::shd{
	void FloatToIntBuffer::createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow){ //FIXME REMOVE LAYOUT
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
}