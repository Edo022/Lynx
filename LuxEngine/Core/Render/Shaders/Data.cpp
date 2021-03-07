#include "LuxEngine/Core/Render/Shaders/Data.hpp"


namespace lux::core::c::shaders{
    //     VkImageMemoryBarrier readToWriteBarrier{ 			        //Create memory barrier object
	// 	.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	//Set structure type
	// 	.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT,				//Set source access mask
	// 	.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,			//Set destination access mask. It must be writable in order to copy the buffer in it
	// 	.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,				//Set old layout. Swapchain images are in undefined layout after being acquired
	// 	.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	//Set new layout. Destination optimal allows the image to be used as a transfer destination
	// 	.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,			    	//Queue families unset
	// 	.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,				    //Queue families unset
	// 	.subresourceRange{												//Create subresource object
	// 		.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,					//Set the aspect mask
	// 		.baseMipLevel   = 0,											//No mipmap
	// 		.levelCount     = 1,											//No multi leve images
	// 		.baseArrayLayer = 0,											//Set base layer
	// 		.layerCount     = 1												//No multi layer
	// 	}
	// };




    // VkImageMemoryBarrier writeToReadBarrier{				       	//Create memory barrier object
    //     .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	//Set structure type
    //     .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,			//Set source access mask
    //     .dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT,				//Set destination access mask. It must be readable to be displayed
    //     .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	//Set old layout. Swapchain images is in dst optimal layout after being written
    //     .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,			//Set new layout. Swapchain images must be in this format to be displayed on screen
    //     .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,			    	//Queue families unset
    //     .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,			    	//Queue families unset
    //     .subresourceRange{												//Create subresource object
    //         .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,					//Set the aspect mask
    //         .baseMipLevel   = 0,											//No mipmap
    //         .levelCount     = 1,											//No multi leve images
    //         .baseArrayLayer = 0,											//Set base layer
    //         .layerCount     = 1												//No multi layer
    //     }
    // };




    // VkBufferImageCopy copyRegion{ 				//Create bufferImageCopy region to copy the buffer into the image
    //     .bufferOffset = 0,							//No buffer offset
    //     .bufferRowLength = 0,						//dark magic
    //     .bufferImageHeight = 0,					    //dark magic
    //     .imageSubresource{							//Create subresource object
    //         .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,   	//Set aspect mask
    //         .mipLevel = 0,								//No mipmap
    //         .baseArrayLayer = 0,						//Set base
    //         .layerCount = 1						    	//No multi layer
    //     },
    //     .imageOffset{ 0, 0, 0 }										//No image offset
    // };
}