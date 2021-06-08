#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Input/Input.hpp"
#include "Lynx/Types/Containers/RaArray.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"







namespace lnx{
	Window::Window(uint32 vWidth, uint32 vHeight) : width{ vWidth }, height{ vHeight } {
		init();
		renderCore.t(renderCore, &lnx::core::RenderCore::run);
		renderCore.t.detach();
	}




	void Window::init() {
		window = glfwCreateWindow((i32)width, (i32)height, "Lynx Engine", nullptr, nullptr);
		switch(glfwCreateWindowSurface(core::dvc::instance, window, nullptr, rcast<vk::SurfaceKHR::CType*>(&surface))){
			case VkResult::VK_SUCCESS: break;
			case VkResult::VK_ERROR_INITIALIZATION_FAILED: dbg::printError("Initialization failed"); break;
			case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT: dbg::printError("Extension not present"); break;
			default: _dbg(dbg::printError("Unknown result")) _rls(noop);
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




	void Window::clear(){
		renderCore.clear();
		core::dvc::instance.destroySurfaceKHR(surface, nullptr);
		glfwDestroyWindow(window);
		initialized = false;
	}




	/**
	 * @brief Waits for the current frame to be rendered and closes the window
	 * Callable by: External threads only
	 * Complexity: O(1)
	 */
	void Window::close(){ //TODO add parameter to not wait for window to close
		running = false;
		while(initialized) thr::self::yield();
		// t.join();
	}




	void Window::qSpawn(obj::Obj_bb* pObject){
		// dbg::checkCond(thr::self::thr() == t.thr, "This function cannot be called by the render thread.");

		renderCore.requests_m.lock();
			pObject->render.updates = pObject->render.updates | obj::UpdateBits::eSpawn;
			renderCore.requests.add(pObject);
		renderCore.requests_m.unlock();
	}
}