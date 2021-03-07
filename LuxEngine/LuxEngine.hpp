#pragma once
#define LUX_H_LUX_ENGINE
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"




namespace lux{
	/**
	 * @brief Starts the Lux Engine
	 */
    void start() {
        Thread coreThr(lux::core::run, L{ false });
        coreThr.detach();
        while(!core::initialized) lux::thr::self::yield();
    }


    /**
     * @brief Kills the Lux Engine and frees all of its resources.
     *      Any object that uses the memory pool, the thread pool or other structures created by the engine will stop working.
     *      Make sure to destroy them or don't use them after calling this function
     */
	void kill(){
        core::running = false;

        core::renderThr.join();
        core::FPSCounterThr.join(); //FIXME REMOVE THREAD
        vkDeviceWaitIdle(core::dvc::compute.LD);

        core::render::cleanup(); core::buffers::cleanup();
		vkDestroyInstance(core::dvc::instance, nullptr);
		glfwTerminate();
    }
}