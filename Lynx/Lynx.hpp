#pragma once
#define LUX_H_LUX_ENGINE
#include "Lynx/Core/Core.hpp"
#include "Lynx/Threads/Thread.hpp"




namespace lux{
	/**
	 * @brief Starts the Lux Engine
	 */
    void start() {
        core::running = true;
        core::inputThr(core::inputLoop);
    }


    /**
     * @brief Kills the Lux Engine and frees all of its resources.
     *      Any object that uses the memory pool, the thread pool or other structures created by the engine will stop working.
     *      Make sure to destroy them or don't use them after calling this function
     */
	void kill(){
        core::running = false;

        core::inputThr.join();
        core::dvc::compute.LD.waitIdle();

        core::render::cleanup();
        core::dvc::instance.destroy(nullptr);
		glfwTerminate();
    }
}