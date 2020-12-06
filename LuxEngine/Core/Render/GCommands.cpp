#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux::core::render::cmd{
	VkCommandPool				singleTimeCommandPool = nullptr;
	RtArray<VkCommandBuffer>	singleTimeCommandBuffers;




	void createGraphicsCommandPool( ) {
		VkCommandPoolCreateInfo poolInfo{
			.sType{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
			.queueFamilyIndex{ dvc::graphics.PD.indices.graphicsFamily }
		};
		luxCheckVk(vkCreateCommandPool(dvc::graphics.LD, &poolInfo, nullptr, &singleTimeCommandPool), "Failed to create graphics command pool");
	}




	//Creates, allocates and begins a command buffer for a single time submit
	//Returns the command buffer
	VkCommandBuffer beginSingleTimeCommands( ) {
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo allocInfo{
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
			.commandPool{ singleTimeCommandPool },
			.level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY },
			.commandBufferCount{ 1 },
		};
		vkAllocateCommandBuffers(dvc::graphics.LD, &allocInfo, &commandBuffer);
		VkCommandBufferBeginInfo beginInfo{
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags{ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT },
		};
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}




	//Ends and submits a single time submit command. Then waits until it's executed and frees its memory
	void endSingleTimeCommands(const VkCommandBuffer vCommandBuffer) {
		vkEndCommandBuffer(vCommandBuffer);
		VkSubmitInfo submitInfo{
			.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
			.commandBufferCount{ 1 },
			.pCommandBuffers = { &vCommandBuffer },
		};
		vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(dvc::graphics.graphicsQueue);
		vkFreeCommandBuffers(dvc::graphics.LD, singleTimeCommandPool, 1, &vCommandBuffer);
	}
}