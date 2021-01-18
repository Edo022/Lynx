// #include "LuxEngine/Math/Trigonometry/GoniometricFunctions.hpp"
#include "LuxEngine/Core/Core.hpp"

#include "LuxEngine/Core/Render/CShader.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/GOutput.hpp"
#include "LuxEngine/Core/Render/Render.hpp"
#include "LuxEngine/Core/Render/GSwapchain.hpp"

#include "LuxEngine/System/System.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/Memory/Gpu/VMemory.hpp"
#include "LuxEngine/Debug/Debug.hpp"

#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux::core{
	double	FPS;
	bool	running;
	bool	useVSync;
	bool	initialized = false;

	VkInstance   				instance 					= (VkInstance)  malloc(sizeof(VkInstance  ));
	VkSurfaceKHR 				surface  					= (VkSurfaceKHR)malloc(sizeof(VkSurfaceKHR));
	VkDebugUtilsMessengerEXT	debugMessenger;

	uint32 						validationLayersNum        	= 1;
	const  char** 				validationLayers         	= { new const char*{ "VK_LAYER_KHRONOS_validation"   }};
	uint32 						requiredDeviceExtensionsNum	= 1;
	const  char** 				requiredDeviceExtensions 	= { new const char*{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }};

	lux::thread FPSCounterThr;
	lux::thread renderThr;








	//-------------------------------------------------------------------------------------------------------------------------------------------//








	//TODO remove
	//Deprecated function
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		win10(return system((c::shaders::shaderPath + "/glslc.exe "                                    + pShaderPath + " -o " + pShaderPath + ".spv").begin( )) == 0;)
		linux(return system((lux::sys::dir::thisDir + "/deps/Linux/Vulkan-1.2.162.0/x86_64/bin/glslc " + pShaderPath + " -o " + pShaderPath + ".spv").begin( )) == 0;)
	}




	void run(bool vUseVSync) {
		running = true;

		//Start init time counter and compile shaders
		//TODO create specific function to get some extensions or all the files
		//TODO internal shader compilation
		LuxTime start = luxStartChrono( );
		// c::shaders::shaderPath = sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/";     //.lib
		c::shaders::shaderPath = sys::dir::thisDir + "/LuxEngine/Contents/shaders/";    //No .lib
		//FIXME DONT USE STD
		try {
			for(const auto& name : std::filesystem::recursive_directory_iterator((char*)c::shaders::shaderPath.begin( ))) {
				String luxStrPath = String((char8*)name.path( ).u8string( ).c_str( )); //FIXMEz
				win10(sys::dir::fixWindowsPath(luxStrPath));
				if(sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
					if(!compileShader(luxStrPath.begin( ))) dbg::printError("compilation error");
					else{ Normal printf("%s", (char*)luxStrPath.begin( )); }
				}
			}
		}
		catch(const std::system_error& e) {
			std::cout << "system_error. code: " << e.code( ) << "\nmessage: " << e.what( ) << '\n';
		}

		//Init
		render::wnd::initWindow( );
		Normal	printf("Creating Instance...                     ");
		render::wnd::createInstance( );
		SuccessNoNl printf("ok");

		render::init(vUseVSync);
		c::init( );

		//Loop
		Success printf("Initialization completed in %f seconds", luxStopChrono(start));
		Success printf("Starting Lux Engine\n");
		mainLoop( );								MainSeparator;

		//Exit
		Normal  printf("Cleaning memory\n");
		render::cleanup( ); c::computeCleanup( );
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(render::wnd::window);
		glfwTerminate( );
	}




	void mainLoop( ) {
		luxDebug(pthread_setname_np(pthread_self(), "Lux | Input"));
		FPSCounterThr(runFPSCounterThr);		//FPSCounterThr.detach( );
		renderThr(runRenderThr);				//renderThr.detach( );
		initialized = true;

		while(!glfwWindowShouldClose(render::wnd::window)) glfwWaitEvents( );
		vkDeviceWaitIdle(dvc::graphics.LD);
	}




	void runRenderThr( ) {
		luxDebug(pthread_setname_np(pthread_self(), "Lux | Render"));
		while(running) {
			LuxTime renderTime = luxStartChrono();
			render::drawFrame( );
			//TODO it does nothing but it's probably important, somehow. dunno
			//vkDeviceWaitIdle(compute.LD);
			FPS = luxStopChrono(renderTime);
		}
	}



	//TODO add FPS limit
	void runFPSCounterThr( ) {
		luxDebug(pthread_setname_np(pthread_self(), "Lux | FPS"));
		while(running) {
			static int delay = 1000;
			sleep(delay);
			printf("FPS: %lf\n", 1/FPS);
		}
	}
}

