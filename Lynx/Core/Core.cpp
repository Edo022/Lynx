﻿#include "Lynx/Core/Core.hpp"

#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"

#include "Lynx/System/System.hpp"
#include "Lynx/Core/Input/Input.hpp"
#include "Lynx/Debug/Debug.hpp"








namespace lux::core{
	alignCache std::atomic<bool>   running;
	alignCache Thread inputThr;




	void inputLoop(){
		_dbg(thr::self::setName("Lux | Main"));
		while(running){
			glfwWaitEvents();
			sleep(0);
		}
	}
}
