#include <vulkan/vulkan.h>
#include <climits>
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.hpp"








namespace lux{
	void Window::run(){
		_dbg(thr::self::setName("App | Window"));
		init();
		draw();
		close();
	}




	void Window::init() {
		//Create default shaders
		CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM);
		core::c::shaders::createDefLayout(LUX_DEF_SHADER_2D_LINE,   4, { 0, 0, 0, 1 }, *this);
		core::c::shaders::createDefLayout(LUX_DEF_SHADER_2D_BORDER, 4, { 0, 0, 0, 1 }, *this);
		core::c::shaders::createDefLayout(LUX_DEF_SHADER_CLEAR,     4, { 0, 0, 0, 0 }, *this);
		//FIXME fix that 01010001 thing



		window = glfwCreateWindow(width, height, "Lux Engine", nullptr, nullptr);
		glfwCreateWindowSurface(core::dvc::instance, window, nullptr, &surface);

		{ //Set icon
			#include "__tmp__IconData.hpp"
			unsigned char* h = (unsigned char*)malloc(64*64*4);	//FIXME REMOVE
  			memcpy(h, h2, 64*64*4);								//FIXME REMOVE
  			for(int i = 0; i < 64*64; ++i) {					//FIXME REMOVE
	  			auto tmp = h[i*4 + 0];
	  			h[i*4 + 0] = h[i*4 + 2];
	  			h[i*4 + 2] = tmp;
  			}
			GLFWimage icon{
				.width = 64,
				.height = 64,
				.pixels = h
			};
			glfwSetWindowIcon(window, 1, &icon);
		}


		glfwSetWindowUserPointer      (window, this);
		glfwSetFramebufferSizeCallback(window, resizeCallback);
		glfwSetCursorPosCallback      (window, input::mouseCursorPosCallback);
		glfwSetMouseButtonCallback    (window, input::mouseButtonCallback);
		glfwSetScrollCallback         (window, input::mouseAxisCallback);
		glfwSetKeyCallback            (window, input::keyCallback);


		swp.bindedWindow = this;
		swp.create(false);


		{ //Initialize window buffers and count
			iOut_g. realloc(1920*2 * 1080 * 4);			//A8  R8  G8  B8  UI
			fOut_g. realloc(1920*2 * 1080 * 4 * 4);		//A32 R32 G32 B32 UF
			zBuff_g.realloc(1920*2 * 1080 * 4);			//A8  R8  G8  B8  UI
			//FIXME ^ those allocations use the default maximum window size to prevent the buffer from getting resized too often
			//FIXME detect size at runtime

			wSize_g.realloc(4 * 2);						//Create cell for window size //TODO use dedicated storage and update every time
			wSize_g.map();
			wSize_g[0] = swp.createInfo.imageExtent.width;		//Set width
			wSize_g[1] = swp.createInfo.imageExtent.height;	//Set height
			wSize_g.unmap();
		}
		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(swp.images.count());	//Resize the command buffer array in the shader
			createDefaultCommandBuffers__();
		}

		clearShader = core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{ fOut_g, iOut_g, zBuff_g, wSize_g },
			LUX_DEF_SHADER_CLEAR, (width * height) / (32 * 32) + 1, 1, 1,
			*this
		);

		initialized = true;
	}








	//Creates the default command buffers used for the render
	void Window::createDefaultCommandBuffers__() { //TODO
		{ //Render command pool
			VkCommandPoolCreateInfo commandPoolCreateInfo = { 						//Create command pool create infos
				.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,			//Set structure type
				.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,	//Command buffers and pool can be reset
				.queueFamilyIndex = core::dvc::compute.PD.indices.computeFamilies[0]	//Set the compute family where to bind the command pool
			};
			dbg::checkVk(vkCreateCommandPool(core::dvc::compute.LD, &commandPoolCreateInfo, nullptr, &commandPool), "Unable to create command pool");
		}




		{ //Copy
			//Create command pool
			VkCommandPoolCreateInfo commandPoolCreateInfo = { 					//Create command pool create infos to create the command pool
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,				//Set structure type
				.flags = 0,															//Default falgs
				.queueFamilyIndex = core::dvc::compute.PD.indices.computeFamilies[0]//Set the compute family where to bind the command pool
			}; //FIXME
			dbg::checkVk(vkCreateCommandPool(core::dvc::compute.LD, &commandPoolCreateInfo, nullptr, &copyCommandPool), "Unable to create command pool");

			//Allocate one command buffer for each swapchain image
			VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 			//Create command buffer allocate infos to allocate the command buffer in the command pool
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,			//Set structure type
				.commandPool = copyCommandPool,										//Set command pool where to allocate the command buffer
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//Set the command buffer as a primary level command buffer
				.commandBufferCount = swp.images.count()
			};
			dbg::checkVk(vkAllocateCommandBuffers(core::dvc::compute.LD, &commandBufferAllocateInfo, copyCommandBuffers.begin()), "Unable to allocate command buffers");




			//Record a present command buffers for each swapchain images
			for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
				//Start recording commands
				VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos to start recording the command buffer
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,			//Set structure type
					.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
				};
				dbg::checkVk(vkBeginCommandBuffer(copyCommandBuffers[imgIndex], &beginInfo), "Unable to begin command buffer recording");


				//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
				readToWriteBarrier.image = swp.images[imgIndex];				//Set swapchain image
				VkPipelineStageFlags 												//Create stage flags
					srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,			//The swapchain image is in color output stage
					dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;							//Change it to transfer stage to copy the buffer in it
				vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &readToWriteBarrier);

				copyRegion.imageExtent = { swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height, 1 };	//Copy the whole buffer
				vkCmdCopyBufferToImage(copyCommandBuffers[imgIndex], iOut_g.cell->csc.buffer, swp.images[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


				//Create a barrier to use the swapchain image as a present source image
				writeToReadBarrier.image = swp.images[imgIndex];			//Set swapchain image
				VkPipelineStageFlags 											//Create stage flags
					srcStage1 = VK_PIPELINE_STAGE_TRANSFER_BIT,						//The image is in transfer stage from the buffer copy
					dstStage1 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;		//Change it to color output to present them
				vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &writeToReadBarrier);

				//End command buffer recording
				dbg::checkVk(vkEndCommandBuffer(copyCommandBuffers[imgIndex]), "Failed to record command buffer");
			}
		}
	}




	void Window::close(){
		running = false;
		t.join();
		wSize_g.free(); fOut_g.free(); iOut_g.free(); zBuff_g.free();
		swp.~Swapchain();
		vkDestroySurfaceKHR(core::dvc::instance, surface, nullptr);
		glfwDestroyWindow(window);
	}




	void Window::add(obj::RenderSpace2D* pRenderSpace) {
		CRenderSpaces.add(pRenderSpace);
		pRenderSpace->init(*this);
	}
}