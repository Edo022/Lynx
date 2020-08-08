

#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Devices.h"




namespace lux::core::g::cmd{
	VkCommandPool			singleTimeCommandPool;
	Array<VkCommandBuffer>	singleTimeCommandBuffers;




	void createGraphicsCommandPool( ) {
		VkCommandPoolCreateInfo poolInfo{ };
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = dvc::graphics.PD.indices.graphicsFamily;

		TryVk(vkCreateCommandPool(dvc::graphics.LD, &poolInfo, nullptr, &singleTimeCommandPool)) printError("Failed to create graphics command pool");
	}




	VkCommandBuffer beginSingleTimeCommands( ) {
		//Allocate command buffer
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo allocInfo{
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
			.commandPool{ singleTimeCommandPool },
			.level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY },
			.commandBufferCount{ 1 },
		};
		vkAllocateCommandBuffers(dvc::graphics.LD, &allocInfo, &commandBuffer);

		//Begin command recording
		VkCommandBufferBeginInfo beginInfo{
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags{ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT },
		};
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		//Return the created command buffer
		return commandBuffer;
	}




	void endSingleTimeCommands(const VkCommandBuffer vCommandBuffer) {
		//End command recording
		vkEndCommandBuffer(vCommandBuffer);

		//Submit command buffer to queue
		VkSubmitInfo submitInfo{
			.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
			.commandBufferCount{ 1 },
			.pCommandBuffers = { &vCommandBuffer },
		};
		vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

		//Free memory
		vkQueueWaitIdle(dvc::graphics.graphicsQueue);
		vkFreeCommandBuffers(dvc::graphics.LD, singleTimeCommandPool, 1, &vCommandBuffer);
	}
}