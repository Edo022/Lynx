// #include "LuxEngine/Math/Trigonometry/GoniometricFunctions.hpp"
#include "LuxEngine/Core/Core.hpp"

#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
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
#include <vulkan/vulkan.h>







namespace lux::core{
	alignCache double        FPS;
	alignCache bool          running;
	alignCache bool          useVSync;
	alignCache bool          initialized = false;

	alignCache VkInstance    instance;
	alignCache VkSurfaceKHR  surface;
	alignCache VkDebugUtilsMessengerEXT debugMessenger;

	alignCache uint32        validationLayersNum         = 1;
	alignCache const  char** validationLayers            = { new const char*{ "VK_LAYER_KHRONOS_validation"   }};
	alignCache uint32        requiredDeviceExtensionsNum = 1;
	alignCache const  char** requiredDeviceExtensions    = { new const char*{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }};

	alignCache lux::Thread   FPSCounterThr;
	alignCache lux::Thread   renderThr;








	//-------------------------------------------------------------------------------------------------------------------------------------------//








	//TODO remove
	//Deprecated function
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		win10(return system((c::shaders::shaderPath + "/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv").begin( )) == 0;)
		//FIXME USE EXE IN DEPS OR COMPILE DIRECTLY
		_linux(return system((lux::sys::dir::thisDir + "/" + getEnginePath() + "/deps/Linux/Vulkan-1.2.162.0/x86_64/bin/glslc " + pShaderPath + " -o " + pShaderPath + ".spv").begin( )) == 0;)
		//TODO add string operator+(char)
	}




	void run(bool vUseVSync) {
		running = true;

		//Start init time counter and compile shaders
		//TODO create specific function to get some extensions or all the files
		//TODO internal shader compilation
		LuxTime start = luxStartChrono( );
		c::shaders::shaderPath = sys::dir::thisDir + "/" + getEnginePath() + "/LuxEngine/Contents/shaders/";


		try {
			for(const auto& name : std::filesystem::recursive_directory_iterator((char*)c::shaders::shaderPath.begin( ))) {
				String luxStrPath = String((char8*)name.path( ).u8string( ).c_str( )); //FIXME
				win10(sys::dir::fixWindowsPath(luxStrPath));
				if(sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
					if(!compileShader(luxStrPath.begin( ))) dbg::printError("compilation error");
					else { Normal printf("%s", (char*)luxStrPath.begin( )); }
				}
			}
		}
		catch(const std::system_error& e) {
			std::cout << "system_error. code: " << e.code( ) << "\nmessage: " << e.what( ) << '\n';
		}

		//Init
		// render::wnd::initWindow( ); //FIXME
		// Normal	printf("Creating Instance...                     "); //FIXME
		// render::wnd::createInstance( ); //FIXME
		// SuccessNoNl printf("ok"); //FIXME

		render::init(vUseVSync); //
		buffers::init( );
		c::shaders::init( );

		//Loop
		Success printf("Initialization completed in %f seconds", luxStopChrono(start));
		Success printf("Starting Lux Engine\n");
		mainLoop( );								MainSeparator;

		//Exit
		Normal  printf("Cleaning memory\n");
		render::cleanup( ); buffers::cleanup( );
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(render::wnd::window);
		glfwTerminate( );
	}




	void mainLoop() {
		luxDebug(thr::self::setName("Lux | Input"));
		FPSCounterThr(runFPSCounterThr);		//FPSCounterThr.detach( );
		renderThr(runRenderThr);				//renderThr.detach( );
		initialized = true;

		while(!glfwWindowShouldClose(render::wnd::window)) glfwWaitEvents( );
		vkDeviceWaitIdle(dvc::graphics.LD);
	}




	void runRenderThr() {
		luxDebug(thr::self::setName("Lux | Render"));
		while(running) {
			LuxTime renderTime = luxStartChrono();
			render::drawFrame( );
			//TODO it does nothing but it's probably important, somehow. dunno
			//vkDeviceWaitIdle(compute.LD);
			FPS = luxStopChrono(renderTime);
		}
	}



	//TODO add FPS limit
	void runFPSCounterThr() {
		luxDebug(thr::self::setName("Lux | FPS"));
		while(running) {
			static int delay = 1000;
			sleep(delay);
			printf("FPS: %lf\n", 1/FPS);
		}
	}
}

