#pragma once
#define LUX_NH_START_CORE
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"
// #include "LuxEngine/Threads/ThreadPool.hpp"



namespace lux::core{
// namespace lux{
    luxAutoInit(LUX_NH_START_CORE) {
        Thread coreThr(lux::core::run, L{ false });
        coreThr.detach();
        while(!initialized) lux::thr::self::yield();
    }
    // void init() {
    //     Thread coreThr(lux::core::run, L{ false });
    //     coreThr.detach();
    //     while(!core::initialized) lux::thr::self::yield();
    // }
}