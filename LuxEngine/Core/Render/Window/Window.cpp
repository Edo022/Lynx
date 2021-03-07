#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Types/VPointer.hpp"
#include "LuxEngine/Debug/Debug.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"







namespace lux{




	// luxAutoInit(LUX_H_WINDOW){
	// 	glfwInit();

	// 	//Extensions
	// 	uint32 glfwExtensionCount;
	// 	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
	// 	//! ^ Freed by GLFW

	// 	const char** extensions = (const char**)malloc(sizeof(const char*) * (glfwExtensionCount _dbg(+ 1)));
	// 	for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions[i] = glfwExtensions[i];		//Save them into an array
	// 	_dbg(extensions[glfwExtensionCount] = (VK_EXT_DEBUG_UTILS_EXTENSION_NAME));				//Add debug extension if in debug mode


	// 	//Create debugCreateInfo structure
	// 	_dbg(VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo);
	// 	_dbg(core::debug::populateDebugMessengerCreateInfo(debugCreateInfo));

	// 	VkApplicationInfo appInfo{
	// 		.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	// 		.pApplicationName   = "LuxEngine",
	// 		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	// 		.pEngineName        = "LuxEngine",
	// 		.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
	// 		.apiVersion         = VK_API_VERSION_1_2
	// 	};

	// 	//Create instance
	// 	VkInstanceCreateInfo createInfo{
	// 		.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	// 		.pNext                    = _dbg((VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo)_rls(nullptr),
	// 		.pApplicationInfo         = &appInfo,
	// 		.enabledLayerCount        = _dbg(core::wnd::validationLayersNum) _rls(0),
	// 		_dbg(.ppEnabledLayerNames = core::wnd::validationLayers,)
	// 		.enabledExtensionCount    = glfwExtensionCount _dbg(+ 1),
	// 		.ppEnabledExtensionNames  = extensions
	// 	};
	// 	//Add validation layers if in debug mode
	// 	#ifdef LUX_DEBUG																	//Search for validation layers
	// 		uint32 layerCount = 0;
	// 		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);						//Get layer count
	// 		RtArray<VkLayerProperties> availableLayers(layerCount);
	// 		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin());		//Get layers
	// 		for(uint32 i = 0; i < core::wnd::validationLayersNum; i++) {							//For every layer,
	// 			for(const auto& layerProperties : availableLayers) {							//Check if it's available
	// 				if(core::wnd::validationLayers[i] == layerProperties.layerName) break;				//If not, exit
	// 				else if(core::wnd::validationLayers[i] == availableLayers.end()->layerName) dbg::printError("Validation layers not available. Cannot run in debug mode");
	// 			}
	// 		}
	// 	#endif

	// 	vkCreateInstance(&createInfo, nullptr, &core::dvc::instance);
	// 	free(extensions);
	// }

	Window window; //TODO REMOVE
	// alignCache GLFWwindow* window                = nullptr;
	// alignCache int32       width                 = 1920 * 2;
	// alignCache int32       height                = 1080;
	// std::mutex	windowResizeFence;
	// alignCache vram::ptr<int32, Ram,  Storage>   wSize_g     = nullptr;
	// alignCache vram::ptr<int32, VRam, Storage>   fOut_G   = nullptr;
	// alignCache vram::ptr<int32, VRam, Storage>   iOut_g = nullptr;
	// alignCache vram::ptr<int32, VRam, Storage>   zBuff_g  = nullptr;




	void Window::initWindow() {
		// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(width, height, "Lux Engine", nullptr, nullptr);
		glfwCreateWindowSurface(core::dvc::instance, window, nullptr, &surface);

		{ //Set icon
			unsigned char h[] = {
				255, 0,   0,   255,
				0,   0,   255, 255,
				255, 0,   0,   255,
				0,   0,   255, 255
			};
			GLFWimage icon{
				.width = 2,
				.height = 2,
				.pixels = h
			};
			glfwSetWindowIcon(window, 1, &icon);
		}


		{ //Set callbacks
			glfwSetWindowUserPointer      (window, this);
			glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

			glfwSetCursorPosCallback      (window, input::mouseCursorPosCallback);
			glfwSetMouseButtonCallback    (window, input::mouseButtonCallback);
			glfwSetScrollCallback         (window, input::mouseAxisCallback);

			glfwSetKeyCallback            (window, input::keyCallback);
		}




			swapchain.bindedWindow = this;
			swapchain.swapchainCreate();





			{ //Initialize window buffers and count
				iOut_g.realloc(width * height * 4); //A8-R8-G8-B8 UI
				fOut_G.  realloc(width * height * 4 * 4); //A32-R32-G32-B32 UF
				zBuff_g. realloc(width * height * 4);
				wSize_g.    realloc(4 * 2);	//Create cell for window size //TODO use dedicated storage and update every time

				wSize_g.map();			//Map window size cell
				wSize_g[0] = swapchain.swapchainExtent.width;				//Set width //FIXME DONT DEPEND ON A WINDOW
				wSize_g[1] = swapchain.swapchainExtent.height;				//Set height //FIXME DONT DEPEND ON A WINDOW
				wSize_g.unmap();										//Unmap
			}
			{ //#LLID CCB0000 Create copy command buffers
				copyCommandBuffers.resize(swapchain.swapchainImages.count());			//Resize the command buffer array in the shader //FIXME DONT DEPEND ON A WINDOW
				// core::c::shaders::createDefaultCommandBuffers();								//Create command buffers and command pool
				createDefaultCommandBuffers__();
			}




		clearShader = core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{
				fOut_G,
				iOut_g,
				zBuff_g,
				wSize_g
			},
			LUX_DEF_SHADER_CLEAR, (width * height) / (32 * 32) + 1, 1, 1
		);







	}

		//This function creates the default command buffers used for the render
	//> Engine internal use
	void Window::createDefaultCommandBuffers__() {
		{ //Render command pool
			VkCommandPoolCreateInfo commandPoolCreateInfo = { 						//Create command pool create infos
				.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,			//Set structure type
				.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,	//Command buffers and pool can be reset
				.queueFamilyIndex = core::dvc::compute.PD.indices.computeFamilies[0]			//Set the compute family where to bind the command pool
			};
			dbg::checkVk(vkCreateCommandPool(core::dvc::compute.LD, &commandPoolCreateInfo, nullptr, &core::c::shaders::commandPool), "Unable to create command pool");
		}




		{ //Copy
			//Create command pool
			static VkCommandPoolCreateInfo commandPoolCreateInfo = { 			//Create command pool create infos to create the command pool
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,				//Set structure type
				.flags = 0															//Default falgs
			};
			commandPoolCreateInfo.queueFamilyIndex = core::dvc::compute.PD.indices.computeFamilies[0];	//Set the compute family where to bind the command pool
			dbg::checkVk(vkCreateCommandPool(core::dvc::compute.LD, &commandPoolCreateInfo, nullptr, &copyCommandPool), "Unable to create command pool");

			//Allocate one command buffer for each swapchain image
			static VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 	//Create command buffer allocate infos to allocate the command buffer in the command pool
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,			//Set structure type
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY							//Set the command buffer as a primary level command buffer
			};
			commandBufferAllocateInfo.commandPool = copyCommandPool;	//Set command pool where to allocate the command buffer
			commandBufferAllocateInfo.commandBufferCount = swapchain.swapchainImages.count(); //FIXME DONT DEPEND ON A WINDOW
			dbg::checkVk(vkAllocateCommandBuffers(core::dvc::compute.LD, &commandBufferAllocateInfo, copyCommandBuffers.begin()), "Unable to allocate command buffers");




			//Record a present command buffers for each swapchain images
			for(uint32 imgIndex = 0; imgIndex < swapchain.swapchainImages.count(); imgIndex++) { //FIXME DONT DEPEND ON A WINDOW
				//Start recording commands
				static VkCommandBufferBeginInfo beginInfo = { 						//Create begin infos to start recording the command buffer
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				//Set structure type
					.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT				//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
				};
				dbg::checkVk(vkBeginCommandBuffer(copyCommandBuffers[imgIndex], &beginInfo), "Unable to begin command buffer recording");


				//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
				readToWriteBarrier.image = swapchain.swapchainImages[imgIndex];	//Set swapchain image //FIXME DONT DEPEND ON A WINDOW
				VkPipelineStageFlags 												//Create stage flags
					srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,			//The swapchain image is in color output stage
					dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;							//Change it to transfer stage to copy the buffer in it
				vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &readToWriteBarrier);

				copyRegion.imageExtent = { swapchain.swapchainExtent.width, swapchain.swapchainExtent.height, 1 };	//Copy the whole buffer //FIXME DONT DEPEND ON A WINDOW
				vkCmdCopyBufferToImage(copyCommandBuffers[imgIndex], iOut_g.cell->csc.buffer, swapchain.swapchainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion); //FIXME DONT DEPEND ON A WINDOW


				//Create a barrier to use the swapchain image as a present source image
				writeToReadBarrier.image = swapchain.swapchainImages[imgIndex];	//Set swapchain image //FIXME DONT DEPEND ON A WINDOW
				VkPipelineStageFlags 											//Create stage flags
					srcStage1 = VK_PIPELINE_STAGE_TRANSFER_BIT,						//The image is in transfer stage from the buffer copy
					dstStage1 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;		//Change it to color output to present them
				vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &writeToReadBarrier);

				//End command buffer recording
				dbg::checkVk(vkEndCommandBuffer(copyCommandBuffers[imgIndex]), "Failed to record command buffer");
			}
		}
	}

}