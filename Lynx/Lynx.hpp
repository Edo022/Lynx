#pragma once
#define LNX_H_LNX_ENGINE
#include "Lynx/Core/Core.hpp"
#include "Lynx/Threads/Thread.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/Render/Render.hpp"



namespace lnx{
	/**
	 * @brief Starts the Lynx Engine
	 */
    void start() {
        core::running = true;
        core::inputThr(core::inputLoop);
    }


    /**
     * @brief Kills the Lynx Engine and frees all of its resources
     *      Any object that uses the memory pool, the thread pool or other structures created by the engine will stop working
     *      Make sure to destroy them or don't use them after calling this function
     */
	void kill(){
        core::running = false;

        core::inputThr.join();
        switch(core::dvc::graphics.ld.waitIdle()){
			case vk::Result::eErrorDeviceLost: dbg::printError("Device lost"); break;
			vkDefaultCases;
        }

        core::render::cleanup();
        core::dvc::instance.destroy(nullptr);
		glfwTerminate();
    }
}