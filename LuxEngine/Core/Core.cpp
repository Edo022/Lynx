// #include "LuxEngine/Math/Trigonometry/GoniometricFunctions.hpp"
#include "LuxEngine/Core/Core.hpp"

#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/GOutput.hpp"
#include "LuxEngine/Core/Render/Render.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"

#include "LuxEngine/System/System.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Debug/Debug.hpp"

#include "LuxEngine/Core/LuxAutoInit.hpp"
#include <vulkan/vulkan.h>







namespace lux::core{
	alignCache double        FPS;
	alignCache bool          running;
	alignCache bool          useVSync;
	alignCache bool          initialized = false;

	alignCache lux::Thread   FPSCounterThr;
	alignCache lux::Thread   renderThr;








	//-------------------------------------------------------------------------------------------------------------------------------------------//








	//TODO remove
	//Deprecated function
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		_wds(return system((c::shaders::shaderPath + "/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv").begin()) == 0;)
		//FIXME USE EXE IN DEPS OR COMPILE DIRECTLY
		_lnx(return system((lux::sys::dir::thisDir + "/" + getEnginePath() + "/deps/Linux/Vulkan-1.2.162.0/x86_64/bin/glslc " + pShaderPath + " -o " + pShaderPath + ".spv").begin()) == 0;)
		//TODO add string operator+(char)
	}




	void run(bool vUseVSync) {
		running = true;

		//Start init time counter and compile shaders
		//TODO create specific function to get some extensions or all the files
		//TODO internal shader compilation
		LuxTime start = luxStartChrono(); //TODO TRACK STATIC INITIALIZATION
		c::shaders::shaderPath = sys::dir::thisDir + "/" + getEnginePath() + "/LuxEngine/Contents/shaders/";


		try {
			for(const auto& name : std::filesystem::recursive_directory_iterator((char*)c::shaders::shaderPath.begin())) {
				String luxStrPath = String((char8*)name.path().u8string().c_str()); //FIXME
				_wds(sys::dir::fixWindowsPath(luxStrPath));
				if(sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
					if(!compileShader(luxStrPath.begin())) dbg::printError("compilation error");
					else { Normal printf("%s", (char*)luxStrPath.begin()); }
				}
			}
		}
		catch(const std::system_error& e) {
			std::cout << "system_error. code: " << e.code() << "\nmessage: " << e.what() << '\n';
		}

		//Init
		// render::wnd::initWindow(); //FIXME
		// Normal	printf("Creating Instance...                     "); //FIXME
		// render::wnd::createInstance(); //FIXME
		// SuccessNoNl printf("ok"); //FIXME

		render::init(vUseVSync); //
		buffers::init();
		c::shaders::init();

		//Loop
		Success printf("Initialization completed in %f seconds", luxStopChrono(start));
		Success printf("Starting Lux Engine\n");
		vkDestroySurfaceKHR(core::dvc::instance, core::dvc::dummySurface, nullptr);
		glfwDestroyWindow(core::dvc::dummyWindow);
		// mainLoop();								MainSeparator;
		FPSCounterThr(runFPSCounterThr);		//FPSCounterThr.detach();
		renderThr(runRenderThr);				//renderThr.detach();
		initialized = true;


		//Exit
		// render::cleanup(); buffers::cleanup();
		// vkDestroyInstance(dvc::instance, nullptr);
		// // glfwDestroyWindow(lux::window.window);
		// glfwTerminate();
	}




	void mainLoop() {
		_dbg(thr::self::setName("Lux | Input"));
		FPSCounterThr(runFPSCounterThr);		//FPSCounterThr.detach();
		renderThr(runRenderThr);				//renderThr.detach();
		initialized = true;

		// while(!glfwWindowShouldClose(lux::window.window)) glfwWaitEvents();
		// while(running) glfwWaitEvents(); //TODO
		// vkDeviceWaitIdle(dvc::graphics.LD);
	}




	void runRenderThr() {
		_dbg(thr::self::setName("Lux | Render"));
		while(running) {
			LuxTime renderTime = luxStartChrono();
			// glfwWaitEvents();
			glfwPollEvents();
			/*" //TODO
				On some platforms, a window move, resize or menu operation will cause event processing to block. T
				his is due to how event processing is designed on those platforms.
				You can use the window refresh callback to redraw the contents of your window when necessary during such operations.
			"*/
			render::drawFrame();
			// vkDeviceWaitIdle(dvc::compute.LD);
			FPS = luxStopChrono(renderTime); //TODO ADD FPS LIMIT
		}
	}



	//FIXME REMOVE THREAD
	void runFPSCounterThr() {
		_dbg(thr::self::setName("Lux | FPS"));
		while(running) {
			static int delay = 1000;
			sleep(delay);
			printf("FPS: %lf\n", 1/FPS);
		}
	}
}

