#include "LuxEngine/Core/Core.hpp"

#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"

#include "LuxEngine/System/System.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Debug/Debug.hpp"








namespace lux::core{
	alignCache std::atomic<bool>   running;
	alignCache Thread inputThr;




	void inputLoop(){
		while(running){
			glfwWaitEvents();
			sleep(0);
		}
	}
}

