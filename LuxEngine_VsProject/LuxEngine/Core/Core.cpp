

#include "LuxEngine/Threads/ThreadPool.h"
#include "LuxEngine/Math/Trigonometry/GoniometricFunctions.h"
#include "LuxEngine/Core/Core.h"

#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Graphics/GOutput.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"

#include "LuxEngine/System/System.h"
#include "Input/Input.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Gpu/VMemory.h"
#include "LuxEngine/Core/ConsoleOutput.h"










#pragma optimize("", off)
PostInitializer(LUX_H_CORE);
#pragma optimize("", on)
namespace lux::core{
	double						NoInitVar(FPS);
	float						NoInitVar(FOV);
	bool						NoInitVar(running);
	bool						NoInitVar(useVSync);
	bool						NoInitVar(initialized);
	uint32						NoInitVar(frames);

	VkInstance					NoInitVar(instance);
	VkDebugUtilsMessengerEXT	NoInitVar(debugMessenger);
	VkSurfaceKHR				NoInitVar(surface);

	Array<const char*, uint32>	NoInitLux(validationLayers);
	Array<const char*, uint32>	NoInitLux(requiredDeviceExtensions);








	//-------------------------------------------------------------------------------------------------------------------------------------------//

	//TODO merge post initializers in a single header





	//TODO fix functions

	void preInit_( ){
		initialized = false;
		frames = 0;
		validationLayers.Array::Array( );			validationLayers = { "VK_LAYER_KHRONOS_validation" };
		requiredDeviceExtensions.Array::Array( );	requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	}


	//Deprecated function
	//TODO remove
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		return system((c::shaders::shaderPath + "/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv").begin( )) == 0;
	}



	//TODO remove debug stuff from release config
	//TODO https://docs.microsoft.com/en-us/cpp/build/how-to-debug-a-release-build?view=vs-2019



	#pragma optimize("g", off)
	void preInit( ){
		static bool once = true;
		if(once){
			once = false;

			ram::init( );
			;	__lp_goniometric_functions_init( );
			;	sys::init( );
			;		thr::preInit( ); thr::init( );
			;			core::preInit_( ); //This
			;			dvc::preInit( );
			;			c::preInit( );
			;			//c::buffers::    preInit( );
			;				c::shaders::preInit( );
			;			g::preInit( );
			;				g::cmd::preInit( );
			;				g::out::preInit( );
			;				g::swapchain::preInit( );
			;				g::wnd::preInit( );
		}
	}




	//Initializes the Lux namespace
	//Don't call this function. Use LuxInit( ) instead
	void init(bool useVSync) {
		static bool once = true;
		if(once){
			once = false;
			//printf("HHHH\n");
			std::thread renderThr(&run, useVSync, 45);
			renderThr.detach( );
			running = true;

			while(!initialized) sleep(10);
		}
	}
	#pragma optimize("g", on)





	void run(bool vUseVSync, float vFOV) {
		//Start init time counter and compile shaders
		//TODO create specific function to get some extensions or all the files
		//TODO internal shader compilation
		LuxTime start = luxStartChrono( );
		c::shaders::shaderPath = sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/";     //.lib
		//c::shaders::shaderPath = sys::dir::thisDir + "/LuxEngine/Contents/shaders/";    //No .lib
		try {
			for(const auto& name : std::filesystem::recursive_directory_iterator(c::shaders::shaderPath.begin( ))) {
				String luxStrPath = String(name.path( ).u8string( ).c_str( )); sys::dir::fixWindowsPath(luxStrPath);
				if(sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
					if(!compileShader(luxStrPath.begin( ))) printError("compilation error", true, -1);
					else Normal printf("%s", luxStrPath.begin( ));
				}
			}
		}
		catch(const std::system_error& e) {
			std::cout << "system_error. code: " << e.code( ) << "\nmessage: " << e.what( ) << '\n';
		}

		//Init
		g::wnd::initWindow( );
		Normal	printf("Creating Instance...                     ");
		g::wnd::createInstance( );
		SuccessNoNl printf("ok");

		g::init(vUseVSync, vFOV);
		//c::buffers::init( );
		//ram is inizialized in init function as it's required for everything
		rem::init( );
		c::init( );

		//Loop
		Success printf("Initialization completed in %f s", luxStopChrono(start));
		Success printf("Starting Lux Engine\n");
		mainLoop( );								MainSeparator;

		//Exit
		Normal  printf("Cleaning memory\n");
		g::cleanup( ); c::computeCleanup( );
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(g::wnd::window);
		glfwTerminate( );
	}




	void mainLoop( ) {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  User input"));
		std::thread FPSCounterThr(&runFPSCounterThr);		FPSCounterThr.detach( );
		std::thread renderThr(&runRenderThr);				renderThr.detach( );
		initialized = true;

		while(!glfwWindowShouldClose(g::wnd::window)) glfwWaitEvents( );
		running = false;
		vkDeviceWaitIdle(dvc::graphics.LD);
	}




	void runRenderThr( ) {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  Render"));
		while(running) {
			g::drawFrame( );
			//TODO it does nothing but it's probably important, somehow. dunno
			//vkDeviceWaitIdle(compute.LD);
			frames++;
		}
	}



	//TODO add FPS limit
	void runFPSCounterThr( ) {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  FPS counter"));
		while(running) {
			static int delay = 1000;
			sleep(delay);
			FPS = frames * (1000 / delay);
			frames = 0;
			printf("FPS: %lf\n", FPS);
		}
	}
}

