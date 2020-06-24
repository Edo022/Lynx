#pragma once
#include "LuxEngine/System/System.h"
#include "LuxEngine/LuxEngine.config.h"
#ifndef __ENGINE
#define __ENGINE






// isShared: 1b
// buffer: 12b
// cellIndex: 20b
// cellSize: 31b
#define __lp_cellCode(isShared, buffer, cellIndex, cellSize) (((uint64)isShared << 63) | ((uint64)(buffer) << 51) | ((uint64)(cellIndex) << 31) | ((cellSize) & 0x7FFFfFFF))
#define __lp_isShared_from_cc(cellCode) (((cellCode) >> 63) & 0b1)
#define __lp_buffer_from_cc(cellCode) (((cellCode) >> 51) & 0xFFF)
#define __lp_cellIndex_from_cc(cellCode) (((cellCode) >> 31) & 0xfFFFF)
#define __lp_cellSize_from_cc(cellCode) ((cellCode) & 0x7FFFffff)

#define __lp_static_buffer_size 50000000 //50MB








//Disabled useless warnings
#pragma warning( disable : 26812 )			//Prefer enum class to enum
#pragma warning( disable : 26495 )			//Uninitialized variables
#pragma warning( disable : 26451 )			//stb arithmetic overflow

//Dark magic
#include <vulkan/vulkan.h>					//Graphics
#include <GLFW/glfw3.h>						//Window system
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1
#include <glm/glm.hpp>						//Shader compatible geometry

//#ifdef LUX_DEBUG //TODO remove

//Default C++
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <set>
//#endif //TODO remove

#include <cmath>
#include <string>

//Junk
#include "LuxEngine/macros.h"				//Useful useless macros
#include <TermColor.hpp>					//Colored console output


//Structures
#include "Graphics/Structs/_VkPhysicalDevice.h"
#include "LuxEngine/Types/Object/Object.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Types/LuxFence.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Math/Trigonometry/GoniometricFunctions.h"


#include "Input/Input.h"


//Re enable warnings for this header
#pragma warning( default : 26451 )			//Arithmetic overflow








//Structures and macros -------------------------------------------------------------------------------------------------------------//








//It's dark magic, idk why or how it works, but it does
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT;
}
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) func(instance, debugMessenger, pAllocator);
}






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









/* ↑↓<>-.'_─│¦
                                                                                                                                                                                        Frame render
                                                                                                                          GPU MEMORY                                                         ↓
                                                                                               ______________________________________________________________________________________________¦_________________________________________________________
                                                                                              │ .────────────────────────────────────────────────────────────────────────────────────────────¦───────────────────────────────────────────────────────. │
   LUX OBJECT DATA MANAGEMENT                                                                 ││                                                                                             ↓                                                        ││
                                                                                              ││       Custom size allocations for large buffers                                          Shaders                   Output buffer (6.2208MB)          ││
                                                                                              ││      .──────────────────────────.  .──────────────────────────.                  .─────────────────────.           Window 0                          ││
   all the buffers are saved as LuxMap s of buffer cellment index                             ││      | Custom size allocation 2 |  | Custom size allocation 3 >-.                │       Shader 0      <--.       .──────────────────────────.       ││
   and allocated in the GPU's memory.                                                         ││      '─────────↑────────────────'  '─────────↑────────────────' '----------------> 9248141834805313536 │  ¦   .---> Custom size allocation 0 >---.   ││
   by default the buffers are not mapped to avoid multi threading issues        .-------------------------------'     Buffer 10 ↑             ¦     Buffer 11 ↑  .---------------->   20266299256898688 │  ¦   ¦   '──────────────────────────'   ¦   ││
                                                                                ¦ .-----------------------------------------------------------'                  ¦                │          ¦          │  ¦   ¦                    Buffer 0 ↑    ¦   ││
   Supported VRAM size: 48GB. 50MB per buffer. max 960 buffers                  ¦ ¦           ││                                                                 ¦                │          ↓          │  ¦   ¦                                  ¦   ││
   Buffer class is just a fancy name for some buffers with the same cell size   ¦ ¦           ││                                                                 ¦                │       Shader 1      <--¦---'    Output buffer (6.2208MB)      ¦   ││
   class 2MB:     25 cells per buffer                                           ¦ ¦           ││       Dynamically allocated buffers                             ¦           .---->   13510803177578784 │  ¦        Window 1                      ¦   ││
   class 500KB:   100 cells per buffer                                          ¦ ¦           ││       cell class 2MB. 25 cells per buffer                 ↑...  ¦           ¦ .-->    9007201402234640 │  ¦       .──────────────────────────.   ¦   ││
   class 5KB:     10k cells per buffer                                          ¦ ¦           ││      .────────────────────────────────────────────────────────. ¦         .-¦ ¦ ->   15763026045542688 │  ¦   - - > Custom size allocation 1 >-. ¦   ││
   class 50B      1Mln cells per buffer                                         ¦ ¦           ││      | cell 24      cell 25      cell 26      ... cell 47     >-'         ¦ ¦ ¦  │          ¦          │  ¦       '──────────────────────────' ¦ ¦   ││
   custom allocation max size: 7FFFFFFF (~2.15GB)                               ¦ ¦           ││      '────────────────────────────────────────────────────────' Buffer 9  ¦ ¦ ¦  │          ↓          │  ¦                        Buffer 1 ↑  ¦ ¦   ││
   larger data structures must be splitted across multiple buffers              ¦ ¦           ││      .────────────────────────────────────────────────────────.           ¦ ¦ ¦  │       Shader 2      <--'                                    ¦ ¦   ││
                                                                                ¦ ¦           ││      | cell 00      cell 01      cell 02      ... cell 23     |           ¦---¦ ->   15763026045542688 │           ↓...                        ¦ ¦   ││
    Cell code limits: max buffer index 4096, max cell index 1 048 576           ¦ ¦           ││      '──────↑────────────↑─────────────↓──────────────────────' Buffer 8  ¦ ¦---->   13510803177578784 │                                       ¦ ¦   ││
    max cell size 2 147 483 648                                                 ¦ ¦     .--------------------'            ¦             '----------------------------------¦ ¦ ¦ ->   18014402806449280 │                                       ¦ ¦   ││
                                                                                ¦ ¦     ¦ .-------------------------------'                                                ¦ ¦ ¦  │          ¦          │                                       ¦ ¦   ││
                                                                                ¦ ¦     ¦ ¦   ││                                                                           ¦ ¦ ¦  │          ↓          │                                       ¦ ¦   ││
 extern                            RAM                                          ¦ ¦     ¦ ¦   ││                                                                           ¦ ¦ ¦  │         ...         │                                       ¦ ¦   ││
   ↓     _______________________________________________________                ¦ ¦     ¦ ¦   ││       Dynamically allocated buffers                                       ¦ ¦ ¦  │   ...               │         //TODO shaders >> copy cmd    ¦ ¦   ││
  Add   │ .───────────────────────────────────────────────────. │               ¦ ¦     ¦ ¦   ││       cell class 500KB. 100 cells per buffer             ↑...             ¦ ¦ ¦  │   ...               │                                       ¦ ¦   ││
   ↓    ││    OBJECT ARRAY             LuxObject               ││               ¦ ¦     ¦ ¦   ││      .────────────────────────────────────────────────────────.           ¦ ¦ ¦  :                     :                                       ¦ ¦   ││
   ¦    ││  .──────────────────.      .────────────────────.   ││               ¦ ¦     ¦ ¦   ││      |cell 100     cell 101     cell 102     ... cell 199     >-----------' ¦ ¦  .                     .                                       ¦ ¦   ││
   ¦    ││  │                  |   .--> vert buffer index  >------13800KB-------' ¦     ¦ ¦   ││      '────────────────────────────────────────────────────────' Buffer 7    ¦ ¦                                                                ¦ ¦   ││
   ¦--------> LuxObject 0      |   ¦--> indx buffer index  >------972KB-----------¦ ----' ¦   ││      .────────────────────────────────────────────────────────.             ¦ ¦                                                                ¦ ¦   ││
   ¦    ││  │ 920k v, 81k t    >---'  │ pos, rot, scl      >-----------------.    ¦       ¦   ││      |cell 000     cell 001     cell 002     ... cell 099     |             ¦ ¦               .----------------------------------------------- ¦-'   ││
   ¦    ││  │                  │      '────────────────────'   ││            ¦    ¦       ¦   ││      '────────────────────────────────↓───────────────────────' Buffer 6    ¦ ¦               ¦                              .-----------------'     ││
   ¦--------> LuxObject 1      │      .────────────────────.   ││            ¦    ¦       ¦   ││                                       ¦                                     ¦ ¦   .───────────↓──────────.       .───────────↓──────────.     ...    ││
   ¦    ││  │ 122k v, 5.2Mln t >---.--> vert buffer index  >------1830KB-----¦ ---¦ ------'   ││                                       '-------------------------------------' ¦   │ Swapchain 0          │       │ Swapchain 1          │            ││
   ¦    ││  │                  │   '--> indx buffer index  >------62400KB----¦ ---'           ││                                                                               ¦   │                      │       │                      │            ││
   ¦--------> LuxObject 2      │      │ pos, rot, scl      >---------------. ¦                ││       Dynamically allocated buffers                                           ¦   │ img >-----.          │       │ img  >-----.         │            ││
   :    ││  │ 6 v, 71 t        >---.  '────────────────────'   ││          ¦ ¦                ││       cell class 5KB. 10k cells per buffer               ↑...                 ¦   │ img >-----¦          │       │ img  >-----¦         │            ││
   .    ││  │ ....             │   ¦  .────────────────────.   ││          ¦ ¦                ││      .────────────────────────────────────────────────────────.               ¦   │ img >-----¦          │       │ img  >-----¦         │            ││
        ││  :                  :   ¦--> vert buffer index  >------90B------¦ ¦ -----.         ││      | cell 10000   cell 10001   cell 10002   ... cell 19999  |               ¦   │ ...       ¦          │       │ ...        ¦         │            ││
        ││  .                  .   '--> indx buffer index  >------852B-----¦ ¦ ---. ¦         ││      '────────────────────────────────────────────────────────' Buffer 5      ¦   '───────────¦──────────'       '────────────¦─────────'            ││
        ││                            │ pos, rot, scl      >-------------. ¦ ¦    ¦ ¦         ││      .────────────────────────────────────────────────────────.               ¦               ¦                               ¦                      ││
        ││                            '────────────────────'   ││        ¦ ¦ ¦    ¦ ¦         ││      | cell 00000   cell 00001   cell 00002   ... cell 09999  |               ¦         .---- ¦ ----------------------------- ¦ ----.                ││
        ││                                                     ││        ¦ ¦ ¦    ¦ ¦         ││      '──────↑────────────↑──↓─────────────────────────────────' Buffer 4      ¦         ¦     ¦                               ¦     ¦                ││
        │'.___________________________________________________.'│        ¦ ¦ ¦    ¦ '------------------------'            ¦  ¦                                                 ¦         ¦                                           ¦                ││
        '───────────────────────────────────────────────────────'        ¦ ¦ ¦    '---------------------------------------'  '-------------------------------------------------'         ¦                                           ¦                ││
                                                                         ¦ ¦ ¦                ││                                                                                         ¦                                           ¦                ││
                                                                         ¦ ¦ ¦                ││                                                                                         ¦            D a r k    M a g i c           ¦                ││
                                                                         ¦ ¦ ¦                ││       Dynamically allocated buffers                                                     ¦                                           ¦                ││
       A            A            A                                       ¦ ¦ ¦                ││       cell class 50B. 1Mln cells per buffer               ↑...                          ¦                                           ¦                ││
       ¦            ¦        B---¦ ->B                                   ¦ ¦ ¦                ││      .────────────────────────────────────────────────────────.                         ¦                                           ¦                ││
   B---¦ ->B    B-->¦-->AB       ¦-->A                                   ¦ ¦ ¦                ││      | cell 1000000 cell 1000001 cell 1000002 ... cell 1999999|                         ¦     ¦                               ¦     ¦                ││
       ↓            ↓            ↓                                       ¦ ¦ ¦                ││      '────────────────────────────────────────────────────────' Buffer 3                '---- ¦ ----------------------------- ¦ ----'                ││
       A            AB           A                                       ¦ ¦ ¦                ││      .────────────────────────────────────────────────────────.                               ¦                               ¦                      ││
                                                                         ¦ ¦ ¦                ││      | cell 0000000 cell 0000001 cell 0000002 ... cell 0999999|                               ¦                               ¦                      ││
                                                                         ¦ ¦ ¦                ││      '──────↑────────────↑────────────↑───────────────────────' Buffer 2                      ¦                               ¦                      ││
                                                                         ¦ ¦ ¦                │'.____________¦____________¦____________¦_______________________________________________________¦_______________________________¦_____________________.'│
                                                                         ¦ ¦ ¦                '──────────────¦────────────¦────────────¦───────────────────────────────────────────────────────¦───────────────────────────────¦───────────────────────'
                                                                         ¦ ¦ '------36B----------------------'            ¦            ¦                                                       ↓                               ↓
                                                                         ¦ '--------36B-----------------------------------'            ¦                                                 Color output                    Color output
                                                                         '----------36B------------------------------------------------'
*/











class Engine {
public:
	double FPS = 0;
	float FOV;
	bool running;
	bool useVSync;
	bool __lp_initialized = false;
	uint32 frame = 0;


	void run(bool vUseVSync, float vFOV);

private:
	//Main
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	//Window
	GLFWwindow* window;								//Main engine window
	int32 width = 1920, height = 1080;
	LuxCell windowSize;
	LuxCell windowOutput;							//The buffer that contains the color output of the window
	LuxCell test___;
	LuxFence windowResizeFence;
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
	void mainLoop();		void FPSCounter();			void render();
	void initWindow();		void initWindowBuffers();	void createInstance();

	//Devices >> Devices.cpp
	void getPhysicalDevices();		void createLogicalDevice(const _VkPhysicalDevice * pPD, VkDevice * pLD, LuxMap<VkQueue>* pComputeQueues);
	static int32 ratePhysicalDevice(const _VkPhysicalDevice* pDevice);
	bool isDeviceSuitable(const VkPhysicalDevice vDevice, LuxString * pErrorText);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device);

	//Shared functions >> this
	uint32* readShaderFromFile(uint32* pLength, const char* pFilePath);
	VkShaderModule createShaderModule(const VkDevice device, uint32* code, const uint32* size);
	void createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);




	// Graphics ---------------------------------------------------------------------------------------------------------------------------------//




	bool framebufferResized = false;				//Updates the swapchain when the window is resized	

	//Swapchain
	VkSwapchainKHR swapChain;
	LuxArray<VkImage> swapChainImages;
	LuxArray<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	LuxArray<VkFramebuffer> swapChainFramebuffers;

	//Render
	VkRenderPass renderPass;
	const int32 MAX_FRAMES_IN_FLIGHT = 16; //Default:2 //TODO ? wtf idk


private:
	//Commands
	VkCommandPool graphicsCommandPool;
	LuxArray<VkCommandBuffer> commandBuffers;

	//Render semaphores and frame
	LuxArray<VkSemaphore> imageAvailableSemaphores;
	LuxArray<VkSemaphore> renderFinishedSemaphores;
	LuxArray<VkFence> inFlightFences;
	LuxArray<VkFence> imagesInFlight;
	int64 currentFrame = 0;

	//debug and validation layers data
	LuxArray<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	LuxArray<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };



	//Graphics >> Graphics/GGraphics.cpp
	void runGraphics(const bool vUseVSync = true, const float vFOV = 45.0f);
	void initVulkan();
	void createSurface();
	void createSyncObjects();
	void createDebugMessenger();
	void drawFrame();
	static void framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight);
	void cleanupGraphics();


	//Graphics swapchain >> Graphics/GSwapchain.cpp
	void createSwapChain();			void recreateSwapChain(bool windowResized);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const LuxArray<VkSurfaceFormatKHR>* availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const LuxArray<VkPresentModeKHR>* availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities);

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		LuxArray<VkSurfaceFormatKHR> formats;
		LuxArray<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createImageViews();
	void cleanupSwapChain();


	//Graphics textures and images >> Graphics/GImages.cpp
	VkImageView createImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);
	void copyBufferToImage(const VkBuffer vBuffer, const  VkImage vImage, const uint32 vWidth, const uint32 vHeight);


	//Graphics commands >> Graphics/GCommands.cpp
	void createGraphicsCommandPool();
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);


	//Graphics render >> Graphics/GPipeline.cpp
	void createRenderPass();
	void createFramebuffers();


	//Graphics other >> Graphics/Graphics.cpp
	VkFormat findSupportedFormat(const LuxArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures);
	uint32 findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties);
	void copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);




	// Compute ----------------------------------------------------------------------------------------------------------------------------------//




	//COMPUTE 
	const int32 WORKGROUP_SIZE = 32; // Workgroup size in compute shader.

									 
	//A container struct for the components of a shader
	struct LuxCShader {
		//Descriptors
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;
		//Pipeline
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		//Commands
		VkCommandPool commandPool;
		LuxArray <VkCommandBuffer> commandBuffers;
	};
	LuxMap<LuxCShader> CShaders;


	//Buffer classes (size of its cells)
	enum LuxBufferClass :uint32 {
		LUX_BUFFER_CLASS_50 = 50,
		LUX_BUFFER_CLASS_5K = 5000,
		LUX_BUFFER_CLASS_500K = 500000,
		LUX_BUFFER_CLASS_2MLN = 2000000,
		LUX_BUFFER_CLASS_LRG = 0
	};
	//This structure groups the components of a Vulkan buffer
	struct _LuxBufferStruc {
		uint32 size;				//The size in bytes of the buffer
		VkBuffer buffer;			//The actual Vulkan buffer
		VkDeviceMemory memory;		//The memory of the buffer
		bool cpuAccessible;
		bool isMapped = false;		//Whether the buffer is mapped or not

		LuxBufferClass bufferClass;	//The class of the buffer
		LuxMap<char> cells;			//This array contains no data. It's used to save the state of a cell (used or free) //TODO use a LuxBitArray
	};
	//This function maps a buffer to a void pointer. Mapping a buffer allows the CPU to access its data
	//Mapping an already mapped buffer will overwrite the old mapping
	//*   buffer: a pointer to a _LuxBufferStruc object. It's the buffer that will be mapped
	//*   returns the void pointer that maps the buffer
	void* mapGpuBuffer(LuxCell cell) {
		LuxBuffer buffer = __lp_buffer_from_cc(cell);
		if (CBuffers[buffer].isMapped) vkUnmapMemory(compute.LD, CBuffers[buffer].memory);
		else CBuffers[buffer].isMapped = true;
		void* data;
		vkMapMemory(compute.LD, CBuffers[buffer].memory, 0, CBuffers[buffer].size, 0, &data);
		return data;
	}
	LuxMap<_LuxBufferStruc> CBuffers;



	//Compute >> Compute/Compute.cpp
	void runCompute();
	void cleanupCompute();
	LuxBuffer createGpuBuffer(const uint64 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible);
	LuxCell createGpuCell(const uint64 vCellSize, const bool vCpuAccessible);
	bool destroyGpuCell(const LuxCell vCell);


	//Compute pipeline and descriptors >> Compute/CShader.cpp
	void CShader_createDescriptorSetLayouts(const LuxArray<LuxCell>* pBufferIndices, const LuxShader vCShader);
	void CShader_createDescriptorSets(const LuxArray<LuxCell>* pCells, const LuxShader vCShader);
	void CShader_createPipeline(const char* shaderPath, const LuxShader vCShader);
	void CShader_createCommandBuffers(const LuxShader vCShader);

	LuxShader CShader_new(LuxArray<LuxCell>* pCells, const char* vShaderPath);
	bool CShader_destroy(const LuxShader vCShader);
};








// Init -------------------------------------------------------------------------------------------------------------------------------------//








extern Engine engine;
#define Frame while(engine.running)


//This function is used by the engine. You shouldn't call it
static void __lp_lux_init_run_thr(bool useVSync) {
	engine.run(useVSync, 45);
}



//This function is used by the engine. You shouldn't call it
static void __lp_luxInit( bool useVSync) {
	std::thread t(__lp_lux_init_run_thr, useVSync);
	t.detach();
	engine.running = true;
}



#endif