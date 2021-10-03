#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/System/System.hpp"
#include "Lynx/Core/Input/Input.hpp"
#include "Lynx/Debug/Debug.hpp"




namespace lnx::core{
	__init_var_set_def(std::atomic<bool>, running){
		pVar = true;
	}
	__init_var_set_def(Thread, inputThr){
		pVar(core::inputLoop);
	}


	/**
	 * @brief //TODO
	 */
	void inputLoop(){
		_dbg(thr::self::setName("Lynx | Main"));
		while(running){
			glfwWaitEvents();
			sleep(0);
		}
	}
}

