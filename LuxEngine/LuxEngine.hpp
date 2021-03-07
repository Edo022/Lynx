#pragma once
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1




#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"


namespace lux{
	//This function starts the Lux Engine and opens a default empty window
    void start() {
        Thread coreThr(lux::core::run, L{ false });
        coreThr.detach();
        while(!core::initialized) lux::thr::self::yield();
    }

	//TODO
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