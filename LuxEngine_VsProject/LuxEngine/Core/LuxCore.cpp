

#include "LuxEngine/Core/Core.h"
#include "Input/Input.h"
#include "LuxEngine/System/System.h"










// Main -------------------------------------------------------------------------------------------------------------------------------------//








namespace lux::_engine {
	//Deprecated function
	//TODO remove
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		lux::String compileShaderCommand = lux::sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv";  //lib
		//lux::String compileShaderCommand = lux::sys::dir::thisDir + "/LuxEngine/Contents/shaders/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv"; //No .lib
		return system(compileShaderCommand.begin()) == 0;
	}
}

namespace lux{
	//This function returns a reference of the engine object
	Engine& getEngine( ){
		static Engine engine_;
		return engine_;
	}
}




//Initializes the engine object and all the Lux namespace
//Don't call this function. Use LuxInit( ) instead
void Engine::init(bool useVSync) {
	static bool once = true;
	if(once){
		once = false;
		lux::sys::__lp_init_system( );
		lux::thr::__lp_init_thread( );
		__lp_goniometric_functions_init( );

		std::thread renderThr(&Engine::run, this, useVSync, 45);
		renderThr.detach( );
		running = true;

		while(!initialized) sleep(10);
	}
}






void Engine::run(bool vUseVSync, float vFOV) {
	//Start init time counter and compile shaders
	//TODO create specific function to get some extensions or all the files
	lux::sys::dir::thisDir;
	LuxTime start = luxStartChrono( );
	lux::core::c::shaderPath = lux::sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/";     //.lib
	//shaderPath = lux::sys::dir::thisDir + "/LuxEngine/Contents/shaders/";    //No .lib
	try {
		for(const auto& name : std::filesystem::recursive_directory_iterator(lux::core::c::shaderPath.begin( ))) {
			lux::String luxStrPath = lux::String(name.path( ).u8string( ).c_str( )); lux::sys::dir::fixWindowsPath(luxStrPath);
			if(lux::sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
				if(!lux::_engine::compileShader(luxStrPath.begin( ))) Exit("compilation error")
				else Normal printf("%s", luxStrPath.begin( ));
			}
		}
	}
	catch(const std::system_error& e) {
		std::cout << "system_error. code: " << e.code( ) << "\nmessage: " << e.what( ) << '\n';
	}

	//Init
	lux::core::g::initWindow();
	Normal printf("Creating Instance...                     ");			lux::core::g::createInstance();						SuccessNoNl printf("ok");
	lux::core::g::graphicsInit(vUseVSync, vFOV);
	lux::core::c::computeInit();

	//Loop
	Success printf("Initialization completed in %f s", luxStopChrono(start));
	Success printf("Starting Lux Engine\n");						mainLoop();									MainSeparator;

	//Exit
	Normal  printf("Cleaning memory");								lux::core::g::graphicsCleanup(); lux::core::c::computeCleanup();		NewLine;
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(lux::core::g::window);
	glfwTerminate();
}




void Engine::mainLoop( ) {
	luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  User input"));
	std::thread FPSCounterThr(&Engine::runFPSCounterThr, this);		FPSCounterThr.detach( );
	std::thread renderThr(&Engine::runRenderThr, this);				renderThr.detach( );
	initialized = true;


	while(!glfwWindowShouldClose(lux::core::g::window)) {
		glfwWaitEvents( );
	}
	running = false;
	vkDeviceWaitIdle(lux::core::g::graphics.LD);
}




void Engine::runRenderThr() {
	luxDebug(SetThreadDescription(GetCurrentThread(), L"\tLuxEngine  |  Render"));
	while (running) {
		lux::core::g::graphicsDrawFrame();
		//TODO it does nothing but it's probably important, somehow. dunno
		//vkDeviceWaitIdle(compute.LD);
		frames++;
	}
}



//TODO add FPS limit
void Engine::runFPSCounterThr() {
	luxDebug(SetThreadDescription(GetCurrentThread(), L"\tLuxEngine  |  FPS counter"));
	while (running) {
		static int delay = 1000;
		sleep(delay);
		FPS = frames * (1000 / delay);
		frames = 0;
		printf("FPS: %lf\n", FPS);
	}
}


