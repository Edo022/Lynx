#pragma once

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
#include "LuxEngine/Types/EngineTypes.h"


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







/*   ↑↓<>-.'_│                                                                                                              GPU MEMORY                                                             
                                                                                               _____________________________________________________________________ 
                                                                                              │ .─────────────────────────────────────────────────────────────────. │                                                                
   LUX OBJECT DATA MANAGEMENT                                                                 ││                                                                   ││                           
                                                                                              ││      .──────────────────────────.  .──────────────────────────.   ││                          
   all the buffers are saved as LuxMap s of buffer cellment index                             ││      | Custom size allocation 0 |  | Custom size allocation 1 |   ││                          
   and allocated in the GPU's memory.                                                         ││      '─────────↑────────────────'  '─────────↑────────────────'   ││                          
   by default the buffers are not mapped to avoid multi threading issues        .-------------------------------'                             ¦                    ││                          
                                                                                ¦ .-----------------------------------------------------------'                    ││                          
   Supported VRAM size: 48GB                                                    ¦ ¦           ││                                                                   ││                          
   50MB per buffer. max 960 buffers                                             ¦ ¦           ││       Dynamically allocated buffers                               ││                          
   class 2MB:     25 cells per buffer                                           ¦ ¦           ││       cell class 2MB. 1Mln cells per buffer                       ││                          
   class 500KB:   100 cells per buffer                                          ¦ ¦           ││      .────────────────────────────────────────────────────────.   ││                          
   class 5KB:     10k cells per buffer                                          ¦ ¦           ││      | cell 24      cell 25      cell 26      ... cell 47     |   ││                          
   class 50B      1Mln cells per buffer                                         ¦ ¦           ││      '────────────────────────────────────────────────────────'   ││                          
   custom allocation max size: 7FFFFFFF (~2.15GB)                               ¦ ¦           ││      .────────────────────────────────────────────────────────.   ││                          
   larger structures have to be splitted across more buffers                    ¦ ¦           ││      | cell 00      cell 01      cell 02      ... cell 23     |   ││                          
                                                                                ¦ ¦           ││      '──────↑────────────↑────────────────────────────────────'   ││                          
                                                                                ¦ ¦     .--------------------'            ¦                                        ││                           
                                                                                ¦ ¦     ¦ .-------------------------------'                                        ││                           
                                                                                ¦ ¦     ¦ ¦   ││                                                                   ││                           
 Extern                            RAM                                          ¦ ¦     ¦ ¦   ││                                                                   ││                           
   ¦     _______________________________________________________                ¦ ¦     ¦ ¦   ││       Dynamically allocated buffers                               ││                           
   ¦    │ .───────────────────────────────────────────────────. │               ¦ ¦     ¦ ¦   ││       cell class 500KB. 1Mln cells per buffer                     ││                           
   ¦    ││                                                     ││               ¦ ¦     ¦ ¦   ││      .────────────────────────────────────────────────────────.   ││                           
   ¦    ││    OBJECT                  .────────────────────.   ││               ¦ ¦     ¦ ¦   ││      |cell 100     cell 101     cell 102     ... cell 199     |   ││                           
   ¦    ││    ARRAY                   │ vert buffer index  >------13800KB-------' ¦     ¦ ¦   ││      '────────────────────────────────────────────────────────'   ││                           
   ¦    ││  .──────────────────.   .--> indx buffer index  >------972KB-----------¦ ----' ¦   ││      .────────────────────────────────────────────────────────.   ││                           
   ¦    ││  │                  |   ¦  │ pos, rot, scl      >-----------------.    ¦       ¦   ││      |cell 000     cell 001     cell 002     ... cell 099     |   ││                           
   ¦--------> LuxObject 0      >---'  '────────────────────'   ││            ¦    ¦       ¦   ││      '────────────────────────────────────────────────────────'   ││                           
   ¦    ││  │ 920k v, 81k t    │      .────────────────────.   ││            ¦    ¦       ¦   ││                                                                   ││                           
   ¦    ││  │                  │      │ vert buffer index  >------1830KB-----¦ ---¦ ------'   ││                                                                   ││                           
   ¦--------> LuxObject 1      >------> indx buffer index  >------62400KB----¦ ---'           ││                                                                   ││                           
   ¦    ││  │ 122k v, 5.2Mln t │      │ pos, rot, scl      >---------------. ¦                ││       Dynamically allocated buffers                               ││                           
   ¦    ││  │                  │      '────────────────────'   ││          ¦ ¦                ││       cell class 5KB. 1Mln cells per buffer                       ││                           
   ¦--------> LuxObject 2      >---.  .────────────────────.   ││          ¦ ¦                ││      .────────────────────────────────────────────────────────.   ││                           
   :    ││  │ 6 v, 71 t        │   ¦  │ vert buffer index  >------90B------¦ ¦ -----.         ││      | cell 10000   cell 10001   cell 10002   ... cell 19999  |   ││                           
   .    ││  │ ....             │   '--> indx buffer index  >------852B-----¦ ¦ ---. ¦         ││      '────────────────────────────────────────────────────────'   ││                           
        ││  :                  :      │ pos, rot, scl      >-------------. ¦ ¦    ¦ ¦         ││      .────────────────────────────────────────────────────────.   ││                           
        ││  .                  .      '────────────────────'   ││        ¦ ¦ ¦    ¦ ¦         ││      | cell 00000   cell 00001   cell 00002   ... cell 09999  |   ││                           
        ││                                                     ││        ¦ ¦ ¦    ¦ ¦         ││      '──────↑────────────↑────────────────────────────────────'   ││                           
        │'.___________________________________________________.'│        ¦ ¦ ¦    ¦ '------------------------'            ¦                                        ││                           
        '───────────────────────────────────────────────────────'        ¦ ¦ ¦    '---------------------------------------'                                        ││                           
                                                                         ¦ ¦ ¦                ││                                                                   ││                           
                                                                         ¦ ¦ ¦                ││       Dynamically allocated buffers                               ││                           
                                                                         ¦ ¦ ¦                ││       cell class 50B. 1Mln cells per buffer                       ││                                    
                                                                         ¦ ¦ ¦                ││      .────────────────────────────────────────────────────────.   ││                                                  
                                                                         ¦ ¦ ¦                ││      | cell 1000000 cell 1000001 cell 1000002 ... cell 1999999|   ││                                                    
                                                                         ¦ ¦ ¦                ││      '────────────────────────────────────────────────────────'   ││                                             
                                                                         ¦ ¦ ¦                ││      .────────────────────────────────────────────────────────.   ││                                                      
                                                                         ¦ ¦ ¦                ││      | cell 0000000 cell 0000001 cell 0000002 ... cell 0999999|   ││                           
                                                                         ¦ ¦ ¦                ││      '──────↑────────────↑────────────↑───────────────────────'   ││                           
                                                                         ¦ ¦ ¦                │'.____________¦____________¦____________¦__________________________.'│                             
                                                                         ¦ ¦ ¦                '──────────────¦────────────¦────────────¦────────────────────────────'                                                       
                                                                         ¦ ¦ '------36B----------------------'            ¦            ¦                                                          
                                                                         ¦ '--------36B-----------------------------------'            ¦                                                         
                                                                         '----------36B------------------------------------------------'                                                        
*/                                                                                                                                                                                                 










class Engine;
static Engine* engine;


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
	struct _LuxCell {
		uint64 ID;					//A unique id, different for each buffer
		uint32 size;				//The size in bytes of the buffer
		VkBuffer buffer;			//The actual Vulkan buffer
		VkDeviceMemory memory;		//The memory of the buffer
		bool isMapped = false;		//Whether the buffer is mapped or not
		uint64 fragmentSize = 0;	//The size of the fragments. 0 means that the buffer is not divided
	};
	//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
	//Mapping an already mapped buffer will overwrite the old mapping
	//*   buffer: a pointer to a _LuxCell object. It's the buffer that will be mapped
	//*   returns the void pointer that maps the buffer
	void* mapGpuBuffer(_LuxCell* buffer) {
		if (buffer->isMapped) vkUnmapMemory(compute.LD, buffer->memory);
		else buffer->isMapped = true;
		void* data;
		vkMapMemory(compute.LD, buffer->memory, 0, buffer->size, 0, &data);
		return data;
	}
	LuxMap<_LuxCell> CGpuBuffers;

	//Compute >> Compute/Compute.cpp
	void runCompute();
	void cleanupCompute();
	LuxCell createGpuBuffer(uint64 size);
	LuxCell createGpuFragmentedBuffer(uint64 size, uint64 fragmentSize);
	LuxCell createGpuSharedBuffer(uint32 cellSize, uint32 cellNum);
	int32 newCShader(LuxArray<LuxCell> buffers, const char* shaderPath);

	//Compute pipeline and descriptors >> Compute/CPipeline.cpp
	void CShader_create_descriptorSetLayouts(LuxArray<LuxCell> bufferIndices, LuxShader CShader);
	void CShader_create_descriptorSets(LuxArray<LuxCell> bufferIndices, LuxShader CShader);
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