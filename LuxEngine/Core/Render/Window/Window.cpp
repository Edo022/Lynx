#include <vulkan/vulkan.hpp>
#include <climits>
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.hpp"

#include "LuxEngine/Contents/shaders/Border2D.hpp"
#include "LuxEngine/Contents/shaders/Line2D.hpp"
#include "LuxEngine/Contents/shaders/FloatToIntBuffer.hpp"







namespace lux{
	void Window::run(){
		_dbg(thr::self::setName("App | Window"));
		init();
		draw();
		close();
	}




	void Window::init() {
		//Create default shaders
		// CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM);
		core::c::shaders::createPipeline(LUX_DEF_SHADER_2D_LINE,   shd::Line2D::layout, *this);
		core::c::shaders::createPipeline(LUX_DEF_SHADER_2D_BORDER, shd::Border2D::layout, *this);
		core::c::shaders::createPipeline(LUX_DEF_SHADER_CLEAR,     shd::FloatToIntBuffer::layout, *this);



		window = glfwCreateWindow(width, height, "Lux Engine", nullptr, nullptr);
		glfwCreateWindowSurface(core::dvc::instance, window, nullptr, rcast<vk::SurfaceKHR::CType*>(&surface));

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
			wSize_g.realloc(/*4 * 2*/16);				//Create cell for window size
			//FIXME rounded up to a multiple of 16, make it automatic

			u32 wSize[2] = { swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height };
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			cb.updateBuffer(wSize_g.cell->csc.buffer, wSize_g.cell->localOffset, wSize_g.cell->cellSize, wSize);
			core::render::cmd::endSingleTimeCommands(cb);
			//FIXME AUTOMATIZE BUFFER UPDATE
			//FIXME UPDATE ALL BUFFERS TOGETHER AFTER A FRAME IS RENDERED
		}
		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(swp.images.count());	//Resize the command buffer array in the shader
			createDefaultCommandBuffers__();
		}


		sh_clear.create(fOut_g, iOut_g, zBuff_g, wSize_g);
		sh_clear.createDescriptorSets();
		sh_clear.createCommandBuffers(LUX_DEF_SHADER_CLEAR, (width * height) / (32 * 32) + 1, 1, 1, *this);

		//FIXME ADD RECREATE FUNCTION TO GENERATED INTERFACES
		initialized = true;
	}








	//Creates the default command buffers used for the render
	void Window::createDefaultCommandBuffers__() { //TODO
		{ //Render command pool
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool create infos
				.setFlags            (vk::CommandPoolCreateFlagBits::eResetCommandBuffer)	//Command buffers and pool can be reset
				.setQueueFamilyIndex (core::dvc::compute.PD.indices.computeFamilies[0])		//Set the compute family where to bind the command pool
			;
			core::dvc::compute.LD.createCommandPool(&commandPoolCreateInfo, nullptr, &commandPool);
		}




		{ //Copy
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool
				.setQueueFamilyIndex (core::dvc::compute.PD.indices.computeFamilies[0])		//Set the compute family where to bind the command pool
			; //FIXME
			core::dvc::compute.LD.createCommandPool(&commandPoolCreateInfo, nullptr, &copyCommandPool);

			auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo() 			//Allocate one command buffer for each swapchain image
				.setCommandPool        (copyCommandPool)									//Set command pool where to allocate the command buffer
				.setLevel              (vk::CommandBufferLevel::ePrimary)					//Set the command buffer as primary level command buffer
				.setCommandBufferCount (swp.images.count())									//Set command buffer count
			;
			core::dvc::compute.LD.allocateCommandBuffers(&commandBufferAllocateInfo, copyCommandBuffers.begin());




			//Record a present command buffers for each swapchain images
			// for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
			for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
				auto beginInfo = vk::CommandBufferBeginInfo() 							//Simultaneous use allows the command buffer to be executed multiple times
					.setFlags (vk::CommandBufferUsageFlagBits::eSimultaneousUse)
				;
				copyCommandBuffers[imgIndex].begin(&beginInfo);							//Start recording commands
					//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
					readToWriteBarrier.image = swp.images[imgIndex].image;					//Set swapchain image
					vk::PipelineStageFlags 													//Create stage flags
						srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput,			//The swapchain image is in color output stage
						dstStage = vk::PipelineStageFlagBits::eTransfer;						//Change it to transfer stage to copy the buffer in it
					copyCommandBuffers[imgIndex].pipelineBarrier(srcStage, dstStage, vk::DependencyFlagBits::eDeviceGroup, 0, nullptr, 0, nullptr, 1, &readToWriteBarrier);
					//! ^ https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDependencyFlagBits.html //FIXME dependency flags was 0 but C++ doesn't allow that

					copyRegion.imageExtent = vk::Extent3D{ swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height, 1 };	//Copy the whole buffer
					copyCommandBuffers[imgIndex].copyBufferToImage(iOut_g.cell->csc.buffer, swp.images[imgIndex].image, vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);


					//Create a barrier to use the swapchain image as a present source image
					writeToReadBarrier.image = swp.images[imgIndex].image;					//Set swapchain image
					vk::PipelineStageFlags 													//Create stage flags
						srcStage1 = vk::PipelineStageFlagBits::eTransfer,						//The image is in transfer stage from the buffer copy
						dstStage1 = vk::PipelineStageFlagBits::eColorAttachmentOutput;			//Change it to color output to present them
					copyCommandBuffers[imgIndex].pipelineBarrier(srcStage1, dstStage1, vk::DependencyFlagBits::eDeviceGroup, 0, nullptr, 0, nullptr, 1, &writeToReadBarrier);
					//! ^ https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDependencyFlagBits.html //FIXME dependency flags was 0 but C++ doesn't allow that
				copyCommandBuffers[imgIndex].end();										//End command buffer recording
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




	void Window::spawn(obj::RenderSpace2D* pRenderSpace) {
		CRenderSpaces.add(pRenderSpace);	//BUG OVER
		pRenderSpace->onSpawn(*this);			//BUG >IN
	}
}