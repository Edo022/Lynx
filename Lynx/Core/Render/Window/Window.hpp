#pragma once
#define LNX_H_WINDOW
#include <GLFW/glfw3.h>
#include <atomic>
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/shaders/FloatToIntBuffer.hpp"
#include "Lynx/Core/Input/MouseInput.hpp"
#include "Lynx/Core/Render/Render.hpp" //TODO rename as RenderCore
// #include "Lynx/Types/Object/Obj_b.hpp"


namespace lnx::obj{
	struct RenderSpace2;
	struct obj_bb;
	struct obj2;
}
namespace lnx{
	//FIXME
	struct InputCallbackQueues{
		struct InputCallbackQueue{
			std::atomic<bool> queued = false;
			RaArray<obj::obj2*> list;
			std::mutex m;
			f32v2 pos;
			inline auto add(obj::obj2* pElm){
				m.lock();
				auto r = list.add(pElm);
				m.unlock();
				return r;
			}
			inline auto invalidate(f32v2 vPos){
				m.lock();
				queued = true;
				pos = vPos;
				m.unlock();
			}
		};
		InputCallbackQueue onClick, onEnter, onExit, onMove, onAxis;
		MouseButton lastMouseButton = MouseButton::eN1;
	};





	class Window{
	public:
	// private:
		GLFWwindow*	window;							//GLFW window object
		uint32		width;							//Width of the window
		uint32 		height;							//Heght of the window
		std::atomic<bool> running     = false;
		std::atomic<bool> initialized = false;

		vk::SurfaceKHR surface;
		core::RenderCore renderCore;

		InputCallbackQueues icQueues;
		void qSpawn(obj::obj_bb* pObject);

		static void __attribute__((used)) resizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight){
			((Window*)glfwGetWindowUserPointer(pWindow))->renderCore.swp.resized = true;
		}

	public:
		Window(uint32 vWidth = 800, uint32 vHeight = 600);
		Window(const Window&) = delete;
		Window(const Window&&) = delete;
		void operator=(const Window&) = delete;
		void operator=(const Window&&) = delete;

		void init();
		void clear();
		void close();

		~Window(){ close(); }
	};
}




/*" //TODO
	On some platforms, a window move, resize or menu operation will cause event processing to block. T
	his is due to how event processing is designed on those platforms.
	You can use the window refresh callback to redraw the contents of your window when necessary during such operations.
"*/