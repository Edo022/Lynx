﻿#pragma once

#ifndef __ENGINE
#define __ENGINE



//Disabled useless warnings
#pragma warning( disable : 26812 )			//Prefer enum class to enum
#pragma warning( disable : 26495 )			//Uninitialized variables
#pragma warning( disable : 6237  )			//Always true enableValidationLayers condition 
#pragma warning( disable : 26451 )			//stb arithmetic overflow

//Dark magic
#include <vulkan/vulkan.h>					//Graphics
#include <GLFW/glfw3.h>						//Window system
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1
#include <glm/glm.hpp>						//Shader compatible geometry


//Default C++
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>
#include <vector>
#include <thread>
#include <set>

#include <cmath>
#include <string>

//Junk
#include "LuxEngine/macros.h"				//Useful useless macros
#include <TermColor.hpp>					//Colored console output


//Structures
#include "Graphics/Structs/_VkPhysicalDevice.h"
#include "Graphics/Structs/Vertex.h"
#include "LuxEngine/Object/Object.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/Containers/LuxArray.h"

#include "Input/Input.h"


//Re enable warnings for this header
#pragma warning( default : 26451 )			//Arithmetic overflow








// Debug, structures and macros -------------------------------------------------------------------------------------------------------------//








//Debug. It's dark magic, idk why or how it works, but it does
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT;
}
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) func(instance, debugMessenger, pAllocator);
}

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	LuxArray<VkSurfaceFormatKHR> formats;
	LuxArray<VkPresentModeKHR> presentModes;
};


//Validation layers in debug mode
#ifdef NDEBUG	
const bool enableValidationLayers = false;
#else			
const bool enableValidationLayers = true;
#endif



#define findDepthFormat()																	\
	findSupportedFormat(																	\
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },\
		VK_IMAGE_TILING_OPTIMAL,															\
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT										\
	);

//Dark magic
#define populateDebugMessengerCreateInfo(createInfo)\
	createInfo = {};\
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;\
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;\
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;\
	createInfo.pfnUserCallback = debugCallback;








// Engine class -----------------------------------------------------------------------------------------------------------------------------//





//         ENGINE CLASS STRUCTURE
//       
//         main()
//         ¦-- run()
//         ¦   │                                                   ___                ___
//         ¦   │-- initWindow()                                       │                  │
//         ¦   │   │-- glfwInit()                                     │                  │
//         ¦   │   │-- set window variable                            │ main             │
//         ¦   │   '-- set window resize callback                     │                  │
//         ¦   │                                                      │                  │
//         ¦   │-- createInstance()                                ___│                  │
//         ¦   │                                                                         │
//         ¦   │                                                   ___                   │
//         ¦   │-- runGraphics()                                      │                  │
//         ¦   │   '-- initVulkan()                                   │                  │
//         ¦   │       │-- createSurface()                            │                  │
//         ¦   │       │-- evalutate physical devices                 │                  │
//         ¦   │       │   │-- save every suitable device data        │                  │
//         ¦   │       │   '-- create logical devices                 │                  │
//         ¦   │       │                                              │                  │ INITIALIZE ENGINE
//         ¦   │       │-- createGraphicsCommandPool()                │                  │
//         ¦   │       │-- createDebugMessenger()                     │ graphics         │
//         ¦   │       │                                              │                  │
//         ¦   │       │-- createTextureImage()                       │                  │
//         ¦   │       │-- createTextureImageView()                   │                  │
//         ¦   │       │-- createTextureSampler()                     │                  │
//         ¦   │       │                                              │                  │
//         ¦   │       │-- createVertexBuffer()                       │                  │
//         ¦   │       │-- createIndexBuffer()                        │                  │
//         ¦   │       │-- ?                                       ___│                  │
//         ¦   │                                                   ___                   │
//         ¦   │-- runCompute()                                       │ compute          │
//         ¦   │   │-- Create image output buffer                     │                  │
//         ¦   │   │-- ?                                           ___│                  │
//         ¦   │                                                   ___                   │
//         ¦   │,- set GLFW keyboard callback                         │ input            │
//         ¦   │'- set GLFW mouse callback                         ___│               ___│
//         ¦   │
//         ¦   │
//         ¦   │  ////////////////////////////////////////////////////////////////////////////////////
//         ¦   │
//         ¦   │                                                   ___                ___
//         ¦   │-- mainLoop()                                         │                  │
//         ¦   │   │-- run fps counter                                │                  │
//         ¦   │   '---every frame                                    │ render loop      │
//         ¦   │   ^   │-- check GLFW events                          │                  │
//         ¦   │   │   │-- render and draw frame to window            │                  │ RUNNING
//         ¦   │   '---'                                           ___│                  │
//         ¦   │                                                                         │
//         '-----> ?                                               ___                   │
//             │                                                   ___│ external      ___│
//             │
//             │
//             │ /////////////////////////////////////////////////////////////////////////////////////
//             │
//             │                                                   ___
//             │,- cleanupGraphics()                                  │ 
//             │'- cleanupCompute()                                   │ 
//             │                                                      │ cleanup
//             │,- destroy instance                                   │
//             │'- destroy window                                     │
//             │                                                      │
//             '-- terminate GLFW                                  ___│










                                                                          
//                                                                                                                                                GPU MEMORY                                    
//                                                                                   ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄                                                                                               
//    LUX OBJECT DATA MANAGEMENT                                                    █                                                                                                                                 █
//                                                                                  █       .──────────────────────────────.                                  .──────────────────────────────.                        █
//    all the buffers are saved as LuxMap s of buffer fragment index                █       | Custom size allocated buffer |                       .----------> Custom size allocated buffer |                        █
//    and allocated in the GPU's memory.                                            █       '──────────────────────────────'                       ¦          '──────────────────────────────'                        █
//    by default the buffers are not mapped to avoid multi threading issues         █                                                              ¦                                                                  █
//                                                                                  █        Dynamically allocated objVsbuffers. 1 million class   ¦          Dynamically allocated index buffers. 1 million class    █
//    objVs<n>: number n object's vertices                                          █        size: 3*(32+8)*1Mln*10 = 1.2Mld b = 150MB/buffer      ¦          size: 3*32*1Mln*10 = 9.6Mld b = 120MB/buffer            █
//    objIs<n>: number n object's indices                                           █       .────────────────────────────────────────────────.     ¦          .────────────────────────────────────────────────.      █
//                                                                                  █       | objVs11   objVs12   objVs13   ...    objVs20   |     ¦          | objIs11   objIs12   objIs13   ...    objIs20   |      █
//    max ~266000 class 10k or                                                      █       '────────────────────────────────────────────────'     ¦          '────────────────────────────────────────────────'      █
//        ~2660 class 100k or                                                       █       .────────────────────────────────────────────────.     ¦          .────────────────────────────────────────────────.      █        
//        ~266 class 1Mln                                                 .-----------------> objVs0    objVs1    objVs2    ...    objVs10   |     ¦          | objIs0    objIs1    objIs2    ...    objIs10   |      █
//                                                                        ¦         █       '─────────────↑──────────────────────────────────'     ¦          '────────────────────────────────────────────────'      █
//                                                                        ¦  .----------------------------┴----------------------------------------'                                                                  █
//                                                                        ¦  ¦      █                                                                                                                                 █
//                                                                        ¦  ¦      █                                                                                                                                 █
//                                                                        ¦  ¦      █        Dynamically allocated objVsbuffers. 100 000 class                Dynamically allocated index buffers. 100 000 class      █
//                                                                        ¦  ¦      █        size: 3*(32+8)*100k*100 = 1.2Mld b = 150MB/buffer                size: 3*32*100k*100 = 9.6Mld b = 120MB/buffer           █
//                                                                        ¦  ¦      █       .────────────────────────────────────────────────.                .────────────────────────────────────────────────.      █
//                                                                        ¦  ¦      █       | objVs101  objVs102  objVs103  ...    objVs200  |                | objIs101  objIs102  objIs103  ...    objIs200  |      █
//                                                                        ¦  ¦      █       '────────────────────────────────────────────────'                '────────────────────────────────────────────────'      █
//                                                                        ¦  ¦      █       .────────────────────────────────────────────────.                .────────────────────────────────────────────────.      █               
//                                                                        ¦  ¦      █       | objVs0    objVs1    objVs2    ...    objVs100  |         .------> objIs0    objIs1    objIs2    ...    objIs100  |      █
//                                                                        ¦  ¦      █       '────────────────────────────────────────────────'         ¦      '────────────────────────────────────────────────'      █
//                                                                        ¦  ¦      █                                                                  ¦                                                              █                  
//                                                                        ├--¦ ------------------------------------------------------------------------'                                                              █                  
//                                      RAM                               ¦  ¦      █                                                                                                                                 █                                                                                                   
//           _________________________________________________________    ¦  ¦      █        Dynamically allocated objVsbuffers. 1000 class                   Dynamically allocated index buffers. 1000 class         █                                                                                     
//         .'                                                         '.  ¦  ¦      █        size: 3*(32+8)*1k*10k = 1.2Mld b = 150MB/buffer                  size: 3*32*1k*10k = 9.6Mld b = 120MB/buffer             █                                                                                                                
//         │                                                           │  ¦  ¦      █       .────────────────────────────────────────────────────.            .────────────────────────────────────────────────────.  █                                                                                                                      
//         │          .────────────────────.       OBJECT              │  ¦  ¦      █       | objVs10001 objVs10002 objVs10003 ...    objVs20000 |            | objIs10001 objIs10002 objIs10003 ...    objIs20000 |  █                                                                                                                        
//         │  Object 0│ vert buffer index  │       ARRAY               │  ¦  ¦      █       '────────────────────────────────────────────────────'            '────────────────────────────────────────────────────'  █                                                                                                                 
//         │  struct  │ indx buffer index  <---. .──────────────────.  │  ¦  ¦      █       .────────────────────────────────────────────────────.            .────────────────────────────────────────────────────.  █                                                                                                                          
//  .-----------------> pos, rot, scl, phs │   ¦ │                  |  │  ¦  ¦      █    .--> objVs0     objVs1     objVs2     ...    objVs10000 |     .------> objIs0     objIs1     objIs2     ...    objIs10000 |  █                                                         
//  ¦      │          '────────────────────'   '-< LuxObject 0      >-----┤  ¦      █    ¦  '────────────────────────────────────────────────────'     ¦      '────────────────────────────────────────────────────'  █                   
//  ¦      │          .────────────────────.     │ 920k v, 81k t    │  │  ¦  ¦      █    ¦                                                             ¦                                                              █                     
//  ¦      │  Object 1│ vert buffer index  │     │                  │  │  '--¦ ----------¦ -----------.                                                ¦                                                              █                     
//  ¦      │  struct  │ indx buffer index  <-----< LuxObject 1      >--------┴-----------¦ -----------¦ -----------------------.                       ¦                                                              █                     
//  ¦ .---------------> pos, rot, scl, phs │     │ 122k v, 5245k t  │  │            █    ¦            ¦                        ¦                       ¦                                                              █                                            
//  ¦ ¦    │          '────────────────────'     │                  │  │            █    ¦            ¦                        ¦                       ¦                                                              █                                            
//  ¦ ¦    │          .────────────────────.   .-< LuxObject 2      >--------------------┴------------¦ -----------------------¦ ----------------------┤                                                              █                                            
//  ¦ ¦    │  Object 2│ vert buffer index  │   ¦ │ 6 v, 71 t        │  │            █                 ¦                        ¦                       ¦                                                              █                                            
//  ¦ ¦    │  struct  │ indx buffer index  <---' │ ....             │  │            █             .───↓────────────────────────↓───────────────────────↓───────────.  allocated for 2Mln objects                      █                                                                                   
//  ¦ ¦ .-------------> pos, rot, scl, phs │     '──────────────────'  │            █  ObjectData │  pos0 32x3b        │      pos1 32x3b       │      pos2  32x3b  │  size = 32*3*3 * 2 000 000                       █                                                                         
//  ¦ ¦ ¦  │          '────────────────────'                           │            █  shared     │  rot0 32x3b        │      rot1 32x3b       │      rot2  32x3b  │  size = 576 000 000 = 72MB                       █                                                                              
//  ¦ ¦ ¦  │                                                           │            █  buffer     │  scl0 32x3b        │      scl1 32x3b       │      scl2  32x3b  │                                                  █                                                                              
//  ¦ ¦ ¦  '._________________________________________________________.'            █             '───↑────────────────────────↑───────────────────────↑───────────'                                                  █                                                                                     
//  ¦ ¦ ¦                                                                           █                 ¦                        ¦                       ¦                                                              █                                                                                     
//  ¦ ¦ ¦                                                                            ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄¦▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄¦▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄¦▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄                                                                                                      
//  ¦ ¦ ¦                                                                                             ¦                        ¦                       ¦                                                              
//  ¦ ¦ ¦                                                             updateFromCpu/Gpu()             ¦                        ¦                       ¦
//  ¦ ¦ '---------------------------------------------------------------------,-----------------------'   updateFromCpu/Gpu()  ¦                       ¦                
//  ¦ '-----------------------------------------------------------------------|-----------------------------------,------------' updateFromCpu/Gpu()   ¦                
//  '-------------------------------------------------------------------------|-----------------------------------|----------------------,-------------'               
//                                                                            |                                   |                      |                            
//                                                                          Extern                              Extern                 Extern                                                  
//                                                                                                                                                             
//                                                                                                                                                             











class Engine;
static Engine* engine;
typedef uint64 LuxShader, LuxGpuBuffer, LuxGpuBufferCell;


class Engine {
public:
	double FPS = 0;
	float FOV;
	bool running;
	bool useVSync;
	bool initialized = false;

	void run(bool _useVSync = true, float FOV = 45.0f);

private:
	//Main
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	//Window
	GLFWwindow* window;								//Main engine's window
	const uint32 WIDTH = 1920, HEIGHT = 1080;		//Default size in windowed mode
	//const uint32 WIDTH = 800, HEIGHT = 600;		//Default size in windowed mode

	//Devices and queues
	struct graphicsDevice {
		_VkPhysicalDevice PD;						//Main physical device for graphics
		VkDevice LD;								//Main logical device for graphics
		VkQueue graphicsQueue;						//Main graphics queue. Runs on graphicsLD
		VkQueue presentQueue;						//Main graphics queue. Runs on graphicsLD
	}graphics;

	struct computeDevice {
		_VkPhysicalDevice PD;						//Main physical device for computing
		VkDevice LD;								//Main logical device for computing
		LuxMap<VkQueue> computeQueues;				//Main compute queues. Run on computeLD
	}compute;

	LuxArray<computeDevice> secondary;				//Secondary devices and queues for computation



	//Main >> this
	void mainLoop();		void FPSCounter(); 
	void initWindow();		void createInstance();

	//Devices >> Devices.cpp
	void getPhysicalDevices();		void createLogicalDevice(_VkPhysicalDevice* PD, VkDevice* LD, VkQueue* graphicsQueue, VkQueue* presentQueue, LuxMap<VkQueue>* computeQueues);
	bool isDeviceSuitable(VkPhysicalDevice device, std::string errorText);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	//Shared functions >> this
	uint32* readShaderFromFile(uint32* length, const char* filename);
	VkShaderModule createShaderModule(VkDevice device, uint32* code, uint32* size);
	void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);




// Graphics ---------------------------------------------------------------------------------------------------------------------------------//




	bool framebufferResized = false;				//Updates the swapchain when the window is resized	
	const char* VERT_PATH = "LuxEngine/Contents/Shaders/vert.spv";
	const char* FRAG_PATH = "LuxEngine/Contents/Shaders/frag.spv";

	//Swapchain
	VkSwapchainKHR swapChain;
	LuxArray<VkImage> swapChainImages;
	LuxArray<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	LuxArray<VkFramebuffer> swapChainFramebuffers;

	//Render
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	const int32 MAX_FRAMES_IN_FLIGHT = 8; //Default:2

public:
	//Geometry
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	LuxArray<Vertex> vertices;
	LuxArray<uint32> indices;

private:
	//Textures
	VkImage textureImage;
	VkImageView textureImageView;
	VkDeviceMemory textureImageMemory;
	VkSampler textureSampler;

	//Images
	VkImage depthImage;
	VkImageView depthImageView;
	VkDeviceMemory depthImageMemory;

	//Descriptors
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	LuxArray<VkDescriptorSet> descriptorSets;

	//Commands
	VkCommandPool graphicsCommandPool;
	LuxArray<VkCommandBuffer> commandBuffers;

	//Render semaphores and frames
	LuxArray<VkSemaphore> imageAvailableSemaphores;
	LuxArray<VkSemaphore> renderFinishedSemaphores;
	LuxArray<VkFence> inFlightFences;
	LuxArray<VkFence> imagesInFlight;
	int64 currentFrame = 0;

	//Debug and validation layers data
	LuxArray<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	LuxArray<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };



	//Graphics >> Graphics/GGraphics.cpp
	void runGraphics(bool _useVSync = true, float FOV = 45.0f);
	void initVulkan();
	void createSurface();
	void createSyncObjects();
	void createDebugMessenger();
	void drawFrame();
	static void framebufferResizeCallback(GLFWwindow* window, int32 width, int32 height);
	void cleanupGraphics();


	//Graphics swapchain >> Graphics/GSwapchain.cpp
	void createSwapChain();			void recreateSwapChain();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(LuxArray<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(LuxArray<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createImageViews();
	void createDepthResources();
	void cleanupSwapChain();


	//Graphics textures and images >> Graphics/GImages.cpp
	void createVertexBuffer();
	void createIndexBuffer();

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void createImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);


	//Graphics commands >> Graphics/GCommands.cpp
	void createGraphicsCommandPool();
	void createDrawCommandBuffers();
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);


	//Graphics render and descriptors >> Graphics/GPipeline.cpp
	void createGraphicsPipeline();
	void createRenderPass();
	void createFramebuffers();

	void createDescriptorPool();
	void createDescriptorSetLayout();
	void createDescriptorSets();


	//Graphics other >> Graphics/Graphics.cpp
	VkFormat findSupportedFormat(LuxArray<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);




// Compute ----------------------------------------------------------------------------------------------------------------------------------//



	
	//COMPUTE 
	const int32 COMPUTE_WIDTH = WIDTH;
	const int32 COMPUTE_HEIGHT = HEIGHT;
	const int32 WORKGROUP_SIZE = 32; // Workgroup size in compute shader.

	// 64 bit integer: 12 for the buffer index (max 4096) and 52 for the fragment index. the size is stored in the buffer
	#define __lp_indicesToMemFrag(buffer, fragmentIndex) (((uint64)buffer << 52) | fragmentIndex)
	#define __lp_FragmentCodeToBuffer(fragmentCode) (fragmentCode >> 52)
	#define __lp_FragmentCodeToFragmentIndex(fragmentCode) (fragmentCode & 0xFFF00000)
	struct Pixel { unsigned char r, g, b, a; };


	VkDebugReportCallbackEXT debugReportCallback;


	//shaders
	struct LuxCShader {
		//Descriptors
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;
		//Pipeline
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		VkShaderModule shaderModule;
		//Commands
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
	};
	LuxMap<LuxCShader> CShaders;


	//Buffers
	//This structure groups the components of a Vulkan buffer
	struct _LuxGpuBuffer {
		uint64 ID;					//A unique id, different for each buffer
		uint32 size;				//The size in bytes of the buffer
		VkBuffer buffer;			//The actual Vulkan buffer
		VkDeviceMemory memory;		//The memory of the buffer
		bool isMapped = false;		//Whether the buffer is mapped or not
		uint64 fragmentSize = 0;	//The size of the fragments. 0 means that the buffer is not divided
	};
	//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
	//Mapping an already mapped buffer will overwrite the old mapping
	//*   buffer: a pointer to a _LuxGpuBuffer object. It's the buffer that will be mapped
	//*   returns the void pointer that maps the buffer
	void* mapGpuBuffer(_LuxGpuBuffer* buffer) {
		if (buffer->isMapped) vkUnmapMemory(compute.LD, buffer->memory);
		else buffer->isMapped = true;
		void* data;
		vkMapMemory(compute.LD, buffer->memory, 0, buffer->size, 0, &data);
		return data;
	}
	LuxMap<_LuxGpuBuffer> CGpuBuffers;

	//Compute >> Compute/Compute.cpp
	void runCompute();
	void cleanupCompute();
	LuxGpuBuffer createGpuBuffer(uint64 size);
	LuxGpuBuffer createGpuFragmentedBuffer(uint64 size, uint64 fragmentSize);
	LuxGpuBuffer createGpuSharedBuffer(uint32 cellSize, uint32 cellNum);
	int32 newCShader(LuxArray<LuxGpuBuffer> buffers, const char* shaderPath);

	//Compute pipeline and descriptors >> Compute/CPipeline.cpp
	void CShader_create_descriptorSetLayouts(LuxArray<LuxGpuBuffer> bufferIndices, LuxShader CShader);
	void CShader_create_descriptorSets(LuxArray<LuxGpuBuffer> bufferIndices, LuxShader CShader);
	void CShader_create_CPipeline(const char* shaderPath, LuxShader CShader);

	//Compute command buffers >> Compute/CCommands.cpp
	void CShader_create_commandBuffer(LuxShader CShader);
	void runCommandBuffer(LuxShader CShader);

};








// Init -------------------------------------------------------------------------------------------------------------------------------------//








#define Frame while(engine->running)


//This function is used by the engine. You shouldn't call it
static void __lp_lux_init_run_thr(bool useVSync) {
	engine->vertices = { Vertex
		{ {-1, -1, 0}, { 1,1,1 }, { 0,0 } },
		{ {-1, 1, 0},	{ 1,1,1 },	{ 0,1 } },
		{ {1, -1, 0},	{ 1,1,1 },	{ 1,0 } },
		{ {1, 1, 0},	{ 1,1,1 },	{ 1,1 } }
	};
	engine->indices = {0, 1, 2, 2, 1, 3};
	engine->run(useVSync, 45);
}



//This function is used by the engine. You shouldn't call it
static void __lp_luxInit(Engine* _engine, bool useVSync) {
	engine = _engine;
	std::thread t(__lp_lux_init_run_thr, useVSync);
	t.detach();
	engine->running = true;
}



#endif