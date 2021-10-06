#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Input/Input.hpp"
#include "Lynx/Types/Containers/RaArray.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"







namespace lnx{
	/**
	 * @brief Resize callback
	 *     This function should only be called by GLFW
	 * Complexity: O(1)
	 */
	void Window::resizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight){
		((Window*)glfwGetWindowUserPointer(pWindow))->renderCore.swp.resized = true;
	}




	/**
	 * @brief Constructor. Initializes the window and starts its main render core
	 * Complexity: O(n + m) [from Window::init]
	 *     where n = core::shaders::pipelineNum, m = number of swapchain images
	 * @param vWidth Initial width of the window
	 * @param vHeight Initial height of the window
	 */
	Window::Window(uint32 vWidth, uint32 vHeight) : width{ vWidth }, height{ vHeight } {
		init();
		renderCore.t(renderCore, &lnx::core::RenderCore::run);
		renderCore.t.detach();
	}




	/**
	 * @brief Initializes the window
	 *     Creates the surface, sets the icon, sets the callbacks and initializes the render core
	 *     This function should only be used by the engine
	 * Complexity: O(n + m) [from RenderCore::init] //TODO
	 *     where n = core::shaders::pipelineNum, m = number of swapchain images
	 */
	void Window::init() {
		window = glfwCreateWindow((i32)width, (i32)height, "Lynx Engine", nullptr, nullptr);
		switch(glfwCreateWindowSurface(core::dvc::g_instance(), window, nullptr, rcast<vk::SurfaceKHR::CType*>(&surface))){
			case VkResult::VK_SUCCESS: break;
			case VkResult::VK_ERROR_INITIALIZATION_FAILED: dbg::logError("Initialization failed"); break;
			case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT: dbg::logError("Extension not present"); break;
			default: _dbg(dbg::logError("Unknown result")) _rls(noop);
		}


		{ //Set icon
			#include "__tmp__IconData.hpp"
			unsigned char* h = (unsigned char*)malloc(64*64*4);	//FIXME REMOVE
  			memcpy(h, h2, 64*64*4);								//FIXME REMOVE
  			for(int i = 0; i < 64*64; ++i) {					//FIXME REMOVE
	  			auto tmp = h[i*4 + 0];
	  			h[i*4 + 0] = h[i*4 + 2];
	  			h[i*4 + 2] = tmp;
  			}
			GLFWimage icon{
				.width = 64,
				.height = 64,
				.pixels = h
			};
			glfwSetWindowIcon(window, 1, &icon);
		}


		glfwSetWindowUserPointer      (window, this);
		glfwSetFramebufferSizeCallback(window, resizeCallback);
		glfwSetCursorPosCallback      (window, input::onMove);
		glfwSetMouseButtonCallback    (window, input::onClick);
		glfwSetScrollCallback         (window, input::onAxis);
		glfwSetCursorEnterCallback    (window, input::onEnter);
		// glfwSetKeyCallback            (window, input::onKey); //FIXME


		renderCore.w = this;
		renderCore.init();

		//FIXME ADD RECREATE FUNCTION TO GENERATED INTERFACES
		initialized = true;
	}




	/**
	 * @brief
	 * Destroys the render core, the surface and the GLFW window and frees any used resource
	 *     This function should only be used by the engine
	 * Complexity: O(n + m) [from RenderCore::clear]
	 *     where n = this->renderCore.swp.images.count() and m = renderMaxFramesInFlight
	 */
	void Window::clear(){
		renderCore.clear();
		core::dvc::g_instance().destroySurfaceKHR(surface, nullptr);
		glfwDestroyWindow(window);
		initialized = false;
	}




	/**
	 * @brief Waits for the current frame to be rendered and closes the window
	 *     This function can only be called by threads that are external to the window
	 * Complexity: Unknown [depends on the state of the internal threads]
	 */
	void Window::close(){ //TODO add parameter to not wait for window to close
		running = false;
		while(initialized) thr::self::yield();
		// t.join();
	}




	/**
	 * @brief Queues an object in the update queue of the render core and switches on its eSpawn bit
	 * Complexity: O(1)
	 * @param pObj The object to spawn
	 */
	void Window::qSpawn(obj::obj_bb* pObj){
		// dbg::checkCond(thr::self::thr() == t.thr, "This function cannot be called by the render thread.");

		renderCore.requests_m.lock();
			pObj->updates = pObj->updates | obj::UpdateBits::eSpawn;
			renderCore.requests.add(pObj);
		renderCore.requests_m.unlock();
	}
}