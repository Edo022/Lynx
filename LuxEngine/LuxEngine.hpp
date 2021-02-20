
#pragma once
//TODO
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1




//UwU


#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"


namespace lux{
    void start() {
        Thread coreThr(lux::core::run, L{ false });
        coreThr.detach();
        while(!core::initialized) lux::thr::self::yield();
    }
}