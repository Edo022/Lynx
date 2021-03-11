#include "LuxEngine/Core/Core.hpp"

#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"

#include "LuxEngine/System/System.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Debug/Debug.hpp"








namespace lux::core{
	alignCache double FPS = 9999;
	alignCache bool   running;
	// alignCache bool   useVSync;

	alignCache lux::Thread FPSCounterThr;
	// alignCache lux::Thread renderThr;








	//-------------------------------------------------------------------------------------------------------------------------------------------//








	void run(bool vUseVSync) {
		running = true;
		LuxTime start = luxStartChrono();

		//TODO count static initialization in init time
		Success printf("Initialization completed in %f seconds\nStarting Lux Engine\n", luxStopChrono(start));

		//Destroy dummy surface and window. Those objects were used during the static initalization, but now they're useless
		// vkDestroySurfaceKHR(core::dvc::instance, core::dvc::dummySurface, nullptr);//FIXME
		// glfwDestroyWindow(core::dvc::dummyWindow);//FIXME

		FPSCounterThr(runFPSCounterThr);
		// renderThr(runRenderThr);
	}




	void runRenderThr() {
		_dbg(thr::self::setName("Lux | Render")); //FIXME REMOVE THREAD
		// while(running) {
		// 	LuxTime renderTime = luxStartChrono();
		// 	glfwPollEvents();
		// 	/*" //TODO
		// 		On some platforms, a window move, resize or menu operation will cause event processing to block. T
		// 		his is due to how event processing is designed on those platforms.
		// 		You can use the window refresh callback to redraw the contents of your window when necessary during such operations.
		// 	"*/
		// 	render::drawFrame(lux::window); //FIXME DONT DEPEND ON A WINDOW
		// 	FPS = luxStopChrono(renderTime); //TODO ADD FPS LIMIT
		// }
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

