#include <vulkan/vulkan.hpp>
#include <climits>
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
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

			wSize_g.realloc(/*4 * 2*/16);						//Create cell for window size //TODO use dedicated storage and update every time
			//FIXME rounded up to a multiple of 16, make it automatic
			// wSize_g.map();
			// wSize_g[0] = swp.createInfo.imageExtent.width;	//Set width
			// wSize_g[1] = swp.createInfo.imageExtent.height;	//Set height
			// wSize_g.unmap();

			u32 wSize[2] = { swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height };
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			cb.updateBuffer(wSize_g.cell->csc.buffer, wSize_g.cell->localOffset, wSize_g.cell->cellSize, wSize);
			core::render::cmd::endSingleTimeCommands(cb);

		}
		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(swp.images.count());	//Resize the command buffer array in the shader
			createDefaultCommandBuffers__();
		}

		// clearShader = core::c::shaders::newShader(
		// 	RtArray<vram::Alloc_b<int32>>{ fOut_g, iOut_g, zBuff_g, wSize_g },
		// 	LUX_DEF_SHADER_CLEAR, (width * height) / (32 * 32) + 1, 1, 1,
		// 	*this
		// );
		sh_clear.create(fOut_g, iOut_g, zBuff_g, wSize_g);
		sh_clear.createDescriptorSets(LUX_DEF_SHADER_CLEAR, *this);
		sh_clear.createCommandBuffers(LUX_DEF_SHADER_CLEAR, (width * height) / (32 * 32) + 1, 1, 1, *this);

//FIXME ADD RECREATE FUNCTION TO GENERATED INTERFACES

//FIXME COMPLETE SHADER REPLACEMENT
		initialized = true;
	}








	//Creates the default command buffers used for the render
	void Window::createDefaultCommandBuffers__() { //TODO
		{ //Render command pool
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 						//Create command pool create infos
				.setFlags            (vk::CommandPoolCreateFlagBits::eResetCommandBuffer)	//Command buffers and pool can be reset
				.setQueueFamilyIndex (core::dvc::compute.PD.indices.computeFamilies[0])	//Set the compute family where to bind the command pool
			;
			core::dvc::compute.LD.createCommandPool(&commandPoolCreateInfo, nullptr, &commandPool);
		}




		{ //Copy
			//Create command pool
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool create infos to create the command pool
				.setQueueFamilyIndex (core::dvc::compute.PD.indices.computeFamilies[0])//Set the compute family where to bind the command pool
			; //FIXME
			core::dvc::compute.LD.createCommandPool(&commandPoolCreateInfo, nullptr, &copyCommandPool);

			//Allocate one command buffer for each swapchain image
			auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo() 			//Create command buffer allocate infos to allocate the command buffer in the command pool
				.setCommandPool        (copyCommandPool)										//Set command pool where to allocate the command buffer
				.setLevel              (vk::CommandBufferLevel::ePrimary)						//Set the command buffer as a primary level command buffer
				.setCommandBufferCount (swp.images.count())
			;
			core::dvc::compute.LD.allocateCommandBuffers(&commandBufferAllocateInfo, copyCommandBuffers.begin());




			//Record a present command buffers for each swapchain images
			// for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
			for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
				//Start recording commands
				auto beginInfo = vk::CommandBufferBeginInfo() 							//Create begin infos to start recording the command buffer
					.setFlags (vk::CommandBufferUsageFlagBits::eSimultaneousUse)			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
				;
				copyCommandBuffers[imgIndex].begin(&beginInfo);


				//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
				readToWriteBarrier.image = swp.images[imgIndex].image;				//Set swapchain image
				vk::PipelineStageFlags 												//Create stage flags
					srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput,			//The swapchain image is in color output stage
					dstStage = vk::PipelineStageFlagBits::eTransfer;							//Change it to transfer stage to copy the buffer in it
				copyCommandBuffers[imgIndex].pipelineBarrier(srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &readToWriteBarrier);

				copyRegion.imageExtent = { swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height, 1 };	//Copy the whole buffer
				copyCommandBuffers[imgIndex].copyBufferToImage(iOut_g.cell->csc.buffer, swp.images[imgIndex].image, vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);


				//Create a barrier to use the swapchain image as a present source image
				writeToReadBarrier.image = swp.images[imgIndex].image;			//Set swapchain image
				vk::PipelineStageFlags 											//Create stage flags
					srcStage1 = vk::PipelineStageFlagBits::eTransfer,						//The image is in transfer stage from the buffer copy
					dstStage1 = vk::PipelineStageFlagBits::eColorAttachmentOutput;		//Change it to color output to present them
				copyCommandBuffers[imgIndex].pipelineBarrier(srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &writeToReadBarrier);

				//End command buffer recording
				copyCommandBuffers[imgIndex].end();
			}
		}
	}




	void Window::close(){
		running = false;
		t.join();
		wSize_g.free(); fOut_g.free(); iOut_g.free(); zBuff_g.free();
		swp.~Swapchain();
		core::dvc::instance.destroySurfaceKHR(surface, nullptr);
		glfwDestroyWindow(window);
	}




	void Window::add(obj::RenderSpace2D* pRenderSpace) {
		CRenderSpaces.add(pRenderSpace);	//BUG OVER
		// sleep(2000);
		pRenderSpace->init(*this);			//BUG >IN
		// sleep(2000);
	}
}