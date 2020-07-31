#pragma once
#ifndef __ENGINE
#define __ENGINE




//TODO check for out of memory error in every result check
//todo add runtime shader compiling support

//Disabled useless warnings
#pragma warning( disable : 26812 )			//Prefer enum class to enum
#pragma warning( disable : 26495 )			//Uninitialized variables
#pragma warning( disable : 26451 )			//stb arithmetic overflow

//Dark magic
#include <GLFW/glfw3.h>						//Window system
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1


#include <deque>

#include "LuxEngine/Types/Containers/LuxDynArray.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Types/Containers/LuxQueue.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"

#include "LuxEngine/System/System.h"
#include "LuxEngine/Math/Trigonometry/GoniometricFunctions.h"
#include "LuxEngine/Threads/ThreadPool.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"


#include "LuxEngine/Engine/Devices_t.h"
#include "LuxEngine/Engine/Compute/CBuffers_t.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"

#include "type_traits"                                 // for move
#include "vcruntime_new.h"                             // for operator delete, operator new
#include "vulkan/vk_platform.h"                        // for VKAPI_ATTR, VKAPI_CALL
#include "vulkan/vulkan_core.h"                        // for VkFence, VkSemaphore, VkCommandBuffer, VkImage, VkPresentModeKHR, VkSurfaceFormatKHR, VkImageView, vkGetInstanceProcAddr, VkFramebuffer, VkBuffer, VkCommandBuffer_T, VkFormat, VkPhysicalDevice, VkDebugUtilsMessengerEXT, VkDevice, VkInstance, VkInstance_T, VkAllocationCallbacks, VkDeviceSize, VkExtent2D, VkMemoryPropertyFlags, VkSurfaceCapabilitiesKHR, PFN_vkCreateDebugUtilsMessengerEXT, PFN_vkDestroyDebugUtilsMessengerEXT, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VkBool32, VkBufferUsageFlags, VkCommandPool, VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, VkDebugUtilsMessengerCallbackDataEXT, VkDebugUtilsMessengerCreateInfoEXT, VkDeviceMemory, VkFormatFeatureFlags, VkImageAspectFlags, VkImageTiling, VkQueue, VkRenderPass, VkResult, VkResult::VK_ERROR_EXTENSION_NOT_PRESENT, VkShaderModule, VkSurfaceKHR, VkSwapchainKHR
#include <thread>

class lux::String;
struct LuxShader_t;
struct RenderSpace2D;



//Re enable warnings for this header
#pragma warning( default : 26451 )			//Arithmetic overflow










// External debug functions ---------------------------------------------------------------------------------------------------------//







class Engine;
namespace lux{ inline Engine& getEngine( ); }
//namespace lux::_engine {
//	inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
//	inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
//	constexpr inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
//}










// Engine class -----------------------------------------------------------------------------------------------------------------------------//





/*	Threads
										 .---> Engine.FPSCounter --.
  E|            Main --> LuxInit --------¦                         ¦
  X|             ¦         ¦             '---> Engine.render       ¦
  E|             ¦         '-Engine.mainLoop-. .---'               ¦
  C|             ¦                           ¦ ¦ .-----------------'
   ↓             :                           ¦ ¦ ¦
				 .                           ¦ ¦ ¦

*/







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
   ¦    ││  .──────────────────.      .────────────────────.   ││               ¦ ¦     ¦ ¦   ││      |cell 100     cell 101     cell 102     ... cell 199     >-----------' ¦ ¦  '                     '                                       ¦ ¦   ││
   ¦    ││  │                  |   .--> vert buffer index  >------13800KB-------' ¦     ¦ ¦   ││      '────────────────────────────────────────────────────────' Buffer 7    ¦ ¦                                                                ¦ ¦   ││
   ¦--------> LuxObject 0      |   ¦--> indx buffer index  >------972KB-----------¦ ----' ¦   ││      .────────────────────────────────────────────────────────.             ¦ ¦                                                                ¦ ¦   ││
   ¦    ││  │ 920k v, 81k t    >---'  │ pos, rot, scl      >-----------------.    ¦       ¦   ││      |cell 000     cell 001     cell 002     ... cell 099     |             ¦ ¦               .----------------------------------------------- ¦-'   ││
   ¦    ││  │                  │      '────────────────────'   ││            ¦    ¦       ¦   ││      '────────────────────────────────↓───────────────────────' Buffer 6    ¦ ¦               ¦                              .-----------------'     ││
   ¦--------> LuxObject 1      │      .────────────────────.   ││            ¦    ¦       ¦   ││                                       ¦                                     ¦ ¦   .───────────↓──────────.       .───────────↓──────────.     ...    ││
   ¦    ││  │ 122k v, 5.2Mln t >---.--> vert buffer index  >------1830KB-----¦ ---¦ ------'   ││                                       '-------------------------------------' ¦   │ Swapchain 0          │       │ Swapchain 1          │            ││
   ¦    ││  │                  │   '--> indx buffer index  >------62400KB----¦ ---'           ││                                                                               ¦   │                      │       │                      │            ││
   ¦--------> LuxObject 2      │      │ pos, rot, scl      >---------------. ¦                ││       Dynamically allocated buffers                                           ¦   │ img >-----.          │       │ img  >-----.         │            ││
   :    ││  │ 6 v, 71 t        >---.  '────────────────────'   ││          ¦ ¦                ││       cell class 5KB. 10k cells per buffer               ↑...                 ¦   │ img >-----¦          │       │ img  >-----¦         │            ││
   '    ││  │ ....             │   ¦  .────────────────────.   ││          ¦ ¦                ││      .────────────────────────────────────────────────────────.               ¦   │ img >-----¦          │       │ img  >-----¦         │            ││
		││  :                  :   ¦--> vert buffer index  >------90B------¦ ¦ -----.         ││      | cell 10000   cell 10001   cell 10002   ... cell 19999  |               ¦   │ ...       ¦          │       │ ...        ¦         │            ││
		││  '                  '   '--> indx buffer index  >------852B-----¦ ¦ ---. ¦         ││      '────────────────────────────────────────────────────────' Buffer 5      ¦   '───────────¦──────────'       '────────────¦─────────'            ││
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



















Object rendering


																							   Render space assembler
																										¦
																										¦
																										¦
																			  Render space assembler    ¦
																						 ¦              ¦
																						 ¦              ¦     Render space assembler
																						 ¦              ¦              ¦
																	SortByZIndex()       ¦              ¦              ¦
								  Objects cache                     ToGlobalPos()        ¦              ¦              ¦
								  .───────────────────.                                  ¦              ¦          .───¦───────────────.
								   \     .─────────.   \                                 ¦              ¦           \  ¦  .─────────.   \
		  Object 0 --> Draw() ----> \     \_________\   \ ----------> --------------> ---¦ ------------------------> \     \_________\   \
		  Ozindex = 2                \   Layer 0         \                               ¦                            \    Layer 0        \
								  .───\                   \                              ¦              ¦  .───────────\                   \
								   \   \___________________\                             ¦              ¦   \          ¦\___________________\
		  Object 1 --> Draw() ----> \                    \ ---------> ---.     .----> ---¦ ----------------> \   Layer ¦ 2  \____\
		  zindex = 0              .──\ Layer 1            \               '. .'          ¦                    \        ¦          \
								   \  \____________________\                X            ¦              ¦      \___________________\───.
									\  Layer 2     \____\                 .' '.          ¦              ¦          \   ¦                \
		  Object 2 --> Draw() -----> \                   \ ---------> ---'     '----> ---¦ -----------------------> \                    \
		  zindex = 1                  \___________________\                              ¦                           \ Layer 1            \
																						 ¦              ¦             \____________________\
																						 ¦              ¦              ¦
																						 ¦              ¦              ¦
																						 ¦              ¦              ¦
	i input                                                                              ¦              ¦              ¦
	u update                                                                             ¦              ¦              ¦
	l draw loop                                                                          ¦              ¦              ¦
	r read, w write                                                                      ¦              ¦              ¦
																						 ¦              ¦              ¦
   2D object                                                                             ¦    .         ↓              ¦              .
	   i, u | in object data | w SHARED                                                  ¦     \ Render space 2        ¦               \
	   i, u | Render object  | r SHARED, w VRAM                                      _ __¦ _    \______________________¦ _______________\                                            Window output                                 RS2
	   l    | Display cache  | w VRAM                                                    ¦  \                          ¦                                                              ___________________________________________________________________
																						 ↓   \     Render space 0      ¦                                                             │            ___________    │ /                                 │   │
   3D object                                                                        Render    \    ____________________¦ _____________________                                       │           │           │   │___________________________________│   │
	   i, u | in object data | w SHARED --> VRAM                                     space 1   \   \                   ↓                      \                                      │   _____   │ /         │                                           │
	   l    | Render object  | w VRAM                                                           \   \      .─ ─ ─ ─ ─ ─ ─ ─ ─ ─.               \                                     │  │     │  │ /         │    ____________________________________   │
																								 \   \      \      .─ ─ ─ ─ ─ ─ ─ ─ ─ ─.        \                                    │  │ /   │  │ /         │   │                                    │  │
																								  \   \      \      \     .─ ─ ─ ─ ─.   \        \  ------> CopyBuffer() --------->  │  │ /   │  │ /         │   │     .───────────────────.          │  │
																								   \   \      \      \     \_ _ _ _ _\   \        \                                  │  │ /   │  │ /         │   │     │      .───────────────────.   │  │
																									\   \      \ _ _ _\                   \        \                                 │  │ /   │  │ /         │   │     │   Lay│     .─────────.   │   │  │
																									 \   \          \  \                   \        \                                │  │ /   │  │ /         │   │     │      │     │_________│   │   │  │
																									  \   \          \  \_ _ _ _ _ _ _ _ _ _\        \                               │  │ /   │  │ /         │   │     │ _____│    Layer 0        │   │  │
																									   \   \          \ _ _ _ _ _ _ _ _ _ _\          \                              │  │ /   │  │           │   │         │  │                   │   │  │
																										\   \                                          \                             │  │ /   │  │           │   │         │ L│___________________│   │  │
																										 \   \                                          \                            │  │     │  │           │   │         │____________________│     │  │
																									_ ____\   \__________________________________________\                           │  │     │  │           │   │                                    │  │
																																													 │  │_____│  │___________│   │____________________________________│  │
																																													 │___________________________________________________________________│
																																														  RS3          RS1                         RS0






*/







enum renderShaderLayout : uint32 {
	LUX_DEF_SHADER_LINE_2D,
	LUX_DEF_SHADER_COPY,
	LUX_DEF_SHADER_NUM
};



//TODO mote to lux::core namespace
class Engine {
public:
	double FPS = 0;
	float FOV;
	bool running;
	bool useVSync;
	bool initialized = false;
	uint32 frames = 0;




	//Main >> this
	VkInstance					instance;
	VkDebugUtilsMessengerEXT	debugMessenger;
	VkSurfaceKHR				surface;
	void init(bool useVSync);
	void run(bool vUseVSync, float vFOV);
	void mainLoop();		void runFPSCounterThr();	void runRenderThr();


	//Window >> this
	GLFWwindow*					window;										//Main engine window
	int32						width = 1920 * 2, height = 1080;			//Size of the window //TODO
	lux::FenceDE				windowResizeFence;
	LuxCell						gpuCellWindowSize;
	LuxCell						gpuCellWindowOutput;						//The buffer that contains the color output of the window
	LuxCell						gpuCellWindowOutput_i;						//The buffer that contains the color output of the window
	void initWindow();		void createInstance();


	//Devices >> Devices.cpp
	graphicsDevice				graphics;									//Main graphics device
	computeDevice				compute;									//Main compute device
	lux::Array<computeDevice>	secondary;									//Secondary compute devices
	void deviceGetPhysical();		void deviceCreateLogical(const _VkPhysicalDevice* pPD, VkDevice* pLD, lux::DynArray<VkQueue>* pComputeQueues);
	static int32		deviceRate(const _VkPhysicalDevice* pDevice);
	bool				deviceIsSuitable(const VkPhysicalDevice vDevice, lux::String* pErrorText);
	bool				deviceCheckExtensions(const VkPhysicalDevice vDevice);
	QueueFamilyIndices	deviceGetQueueFamilies(const VkPhysicalDevice vDevice);


	//Shared functions >> this
	uint32*				cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	VkShaderModule		cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength);
	void				createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);
	void				copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize);











	// Graphics ---------------------------------------------------------------------------------------------------------------------------------//








	//debug and validation layers data
	lux::Array<const char*, uint32> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	lux::Array<const char*, uint32> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };



	//Graphics >> Graphics/GGraphics.cpp
	lux::Array<VkSemaphore>		drawFrameImageAquiredSemaphore;
	lux::Array<VkSemaphore>		drawFrameObjectsRenderedSemaphore;
	lux::Array<VkSemaphore>		drawFrameCopySemaphore;
	lux::Array<VkSemaphore>		drawFrameClearSemaphore;
	lux::Array<VkFence>			drawFrameImageRenderedFence;
	int32						renderCurrentFrame = 0;
	void						graphicsInit(const bool vUseVSync = true, const float vFOV = 45.0f);
	void						graphicsCreateSyncObjs();
	void						graphicsCreateDebugMessenger();
	void						graphicsDrawFrame();
	void						graphicsCleanup();
	static void					framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight);


	//Graphics swapchain >> Graphics/GSwapchain.cpp
	VkSwapchainKHR				swapchain;
	lux::Array<VkImage>			swapchainImages;
	lux::Array<VkImageView>		swapchainImageViews;
	VkFormat					swapchainImageFormat;
	VkExtent2D					swapchainExtent;
	lux::Array<VkFramebuffer>	swapchainFramebuffers;
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		lux::Array<VkSurfaceFormatKHR>	formats;
		lux::Array<VkPresentModeKHR>	presentModes;
	};
	void						swapchainCreate();
	void						swapchainRecreate(const bool vWindowResized);
	void						swapchainCleanup();
	VkSurfaceFormatKHR			swapchainChooseSurfaceFormat(const lux::Array<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR			swapchainChoosePresentMode(const lux::Array<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D					swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails		swapchainQuerySupport(const VkPhysicalDevice vDevice);



	//Graphics images and output objects >> Graphics/GOutput.cpp
	VkRenderPass				renderPass;
	const int32					renderMaxFramesInFlight = 4;		//Default:2
	bool						renderFramebufferResized = false;	//Updates the swapchain when the window is resized
	void						createRenderPass();
	void						createFramebuffers();
	VkImageView					swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags);


	//Graphics commands >> Graphics/GCommands.cpp
	VkCommandPool				singleTimeCommandPool;
	lux::Array<VkCommandBuffer>	singleTimeCommandBuffers;
	void						createGraphicsCommandPool();
	VkCommandBuffer				beginSingleTimeCommands();
	void						endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);


	//Graphics other >> Graphics/Graphics.cpp
	VkFormat					graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures);
	uint32						graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties);
	static VKAPI_ATTR VkBool32 VKAPI_CALL graphicsDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);








	// Compute ----------------------------------------------------------------------------------------------------------------------------------//



	//TODO reuse objects dont destroy them
	//TODO restore() function to restore an object and reuse it instead of destroying it
	//TODO restore function tipo in destroy function description


	//Compute >> Compute/Compute.cpp
	VkCommandPool				copyCommandPool;
	lux::Array<VkCommandBuffer>	copyCommandBuffers;
	VkCommandBuffer				clearCommandBuffer;
	void						computeInit();
	void						computeCleanup();



	//Buffers >> Compute/Buffers.cpp
	lux::Map<LuxBuffer_t, uint32>	CBuffers;				//List of GPU buffers
	LuxBuffer						gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible);
	LuxCell							gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible);
	bool							gpuCellDestroy(const LuxCell vCell);
	void*							gpuCellMap(const LuxCell vCell);



	//Compute pipeline, descriptors and shaders >> Compute/CShader.cpp
	LuxShader ls0;
	LuxShader ls1;
	lux::Map<lux::obj::RenderSpace2D*, uint32>	CRenderSpaces;			//List of renderSpaces
	//lux::Map<lux::obj::Base*, uint32> objs;		//TODO
	lux::String shaderPath;
	lux::Array<LuxShaderLayout_t>	CShadersLayouts;		//Layout of the render shaders
	lux::Map<LuxShader_t, uint32>	CShaders;					//Per-object shaders
	lux::FenceDE addShaderFence;
	void		cshaderCreateDefaultCommandBuffers();
	void		cshaderCreateDefLayout(const renderShaderLayout vRenderShader, const uint32 pCellNum);
	void		cshaderCreateDescriptorSets(LuxShader_t* pCShader, const lux::Array<LuxCell>& pCells, renderShaderLayout vRenderShader);
	void		cshaderCreateCommandBuffers(LuxShader_t* pCShader, const renderShaderLayout vRenderShader, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);
	int32		cshaderNew(const lux::Array<LuxCell>& pCells, const renderShaderLayout vRenderShader, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);
	bool		cshaderDestroy(const LuxShader vCShader);
};

#endif












namespace lux::_engine {
	//It's dark magic, idk why or how it works, but it does
	inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if(func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if(func != nullptr) func(instance, debugMessenger, pAllocator);
	}


	//More dark magic
	static constexpr inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = { };
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = lux::getEngine( ).graphicsDebugCallback;
	}
}











//TODO fast memcpy codes
//#include <immintrin.h>
//#include <assert.h>
//#include <cstdint>
///* ... */
//void fastMemcpy(void *pvDest, void *pvSrc, size_t nBytes) {
//  assert(nBytes % 32 == 0);
//  assert((intptr_t(pvDest) & 31) == 0);
//  assert((intptr_t(pvSrc) & 31) == 0);
//  const __m256i *pSrc = reinterpret_cast<const __m256i*>(pvSrc);
//  __m256i *pDest = reinterpret_cast<__m256i*>(pvDest);
//  int64_t nVects = nBytes / sizeof(*pSrc);
//  for (; nVects > 0; nVects--, pSrc++, pDest++) {
//    const __m256i loaded = _mm256_stream_load_si256(pSrc);
//    _mm256_stream_si256(pDest, loaded);
//  }
//  _mm_sfence();
//}


//"
//On Ryzen 1800X with single memory channel filled completely (2 slots, 16 GB DDR4 in each), the following code is 1.56 times faster than memcpy() on MSVC++2017 compiler.
//If you fill both memory channels with 2 DDR4 modules, i.e. you have all 4 DDR4 slots busy, you may get further 2 times faster memory copying.
//For triple-(quad-)channel memory systems, you can get further 1.5(2.0) times faster memory copying if the code is extended to analogous AVX512 code.
//With AVX2-only triple/quad channel systems with all slots busy are not expected to be faster because to load them fully you need to load/store more than 32 bytes at once
//(48 bytes for triple- and 64-bytes for quad-channel systems), while AVX2 can load/store no more than 32 bytes at once.
//Though multithreading on some systems can alleviate this without AVX512 or even AVX2.
//
//So here is the copy code that assumes you are copying a large block of memory whose size is a multiple of 32 and the block is 32-byte aligned.
//
//For non-multiple size and non-aligned blocks, prologue/epilogue code can be written reducing the width to 16 (SSE4.1), 8, 4, 2 and finally 1 byte at once for the block head and tail.
//Also in the middle a local array of 2-3 __m256i values can be used as a proxy between aligned reads from the source and aligned writes to the destination.
//"









//TODO
//Interpolated point mesh rig