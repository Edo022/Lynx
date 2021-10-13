#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/System/System.hpp"
#include "Lynx/Core/Input/Input.hpp"
#include "Lynx/Debug/Debug.hpp"




namespace lnx::core{
	_lnx_init_var_redirect_value_def((std::atomic<bool>), running, lnx::core){
		*pVar = true;
	}
	_lnx_init_var_redirect_value_def((Thread), inputThr, lnx::core){
		(*pVar)(core::inputLoop);
	}


	/**
	 * @brief //TODO
	 */
	void inputLoop(){
		_dbg(thr::self::setName("Lynx | Main"));
		while(g_running()){
			glfwWaitEvents();
			sleep(0);
		}
	}
}

