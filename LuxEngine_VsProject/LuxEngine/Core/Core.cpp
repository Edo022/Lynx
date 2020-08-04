

#include "LuxEngine/Threads/ThreadPool.h"
#include "LuxEngine/Math/Trigonometry/GoniometricFunctions.h"
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/System/System.h"
#include "Input/Input.h"
#include "LuxEngine/Types/Integers/Integers.h"











namespace lux::core{
	double						FPS;
	float						FOV;
	bool						running;
	bool						useVSync;
	bool						initialized = false;
	uint32						frames = 0;

	VkInstance					instance;
	VkDebugUtilsMessengerEXT	debugMessenger;
	VkSurfaceKHR				surface;

	Array<const char*, uint32>	validationLayers = { "VK_LAYER_KHRONOS_validation" };
	Array<const char*, uint32>	requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


	// Main -------------------------------------------------------------------------------------------------------------------------------------//







	//Deprecated function
	//TODO remove
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		lux::String compileShaderCommand = lux::sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv";  //lib
		//lux::String compileShaderCommand = lux::sys::dir::thisDir + "/LuxEngine/Contents/shaders/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv"; //No .lib
		return system(compileShaderCommand.begin( )) == 0;
	}






	//Initializes the engine object and all the Lux namespace
	//Don't call this function. Use LuxInit( ) instead
	void init(bool useVSync) {
		static bool once = true;
		if(once){
			once = false;
			sys::__lp_init_system( );
			thr::__lp_init_thread( );
			__lp_goniometric_functions_init( );

			std::thread renderThr(&run, useVSync, 45);
			renderThr.detach( );
			running = true;

			while(!initialized) sleep(10);
		}
	}






	void run(bool vUseVSync, float vFOV) {
		//Start init time counter and compile shaders
		//TODO create specific function to get some extensions or all the files
		sys::dir::thisDir;
		LuxTime start = luxStartChrono( );
		c::shaders::shaderPath = sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/";     //.lib
		//shaderPath = sys::dir::thisDir + "/LuxEngine/Contents/shaders/";    //No .lib
		try {
			for(const auto& name : std::filesystem::recursive_directory_iterator(c::shaders::shaderPath.begin( ))) {
				String luxStrPath = String(name.path( ).u8string( ).c_str( )); sys::dir::fixWindowsPath(luxStrPath);
				if(sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
					if(!compileShader(luxStrPath.begin( ))) perror("compilation error");
					else Normal printf("%s", luxStrPath.begin( ));
				}
			}
		}
		catch(const std::system_error& e) {
			std::cout << "system_error. code: " << e.code( ) << "\nmessage: " << e.what( ) << '\n';
		}

		//Init
		g::wnd::initWindow( );
		Normal printf("Creating Instance...                     ");			g::wnd::createInstance( );						SuccessNoNl printf("ok");
		g::graphicsInit(vUseVSync, vFOV);
		c::computeInit( );

		//Loop
		Success printf("Initialization completed in %f s", luxStopChrono(start));
		Success printf("Starting Lux Engine\n");						mainLoop( );									MainSeparator;

		//Exit
		Normal  printf("Cleaning memory");								g::graphicsCleanup( ); c::computeCleanup( );		NewLine;
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(g::wnd::window);
		glfwTerminate( );
	}




	void mainLoop( ) {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  User input"));
		std::thread FPSCounterThr(&runFPSCounterThr);		FPSCounterThr.detach( );
		std::thread renderThr(&runRenderThr);				renderThr.detach( );
		initialized = true;


		while(!glfwWindowShouldClose(g::wnd::window)) {
			glfwWaitEvents( );
		}
		running = false;
		vkDeviceWaitIdle(dvc::graphics.LD);
	}




	void runRenderThr( ) {
		luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  Render"));
		while(running) {
			g::graphicsDrawFrame( );
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









	//Prints an error in the console
	//*   pMessage    | the error message
	//*   vFatalError | if true, the engine will stop its execution
	//*   vErrorCode  | the code the process will exit with. It will also be displayed in the console
	void perror(const String& pMessage, const bool vFatalError, const int32 vErrorCode){
		running = false;
		if(vFatalError) { Failure printf("Fatal error:\nCode %d\n", vErrorCode); }
		else { Failure printf("Error:\nCode %d\n", vErrorCode); }
		Failure printf("\"%s\"", pMessage.begin( ));
		if(!vFatalError) return;
		system("pause"); exit(vErrorCode);
	}
}


