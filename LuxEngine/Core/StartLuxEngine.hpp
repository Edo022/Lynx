#pragma once
#define LUX_NH_START_CORE
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"
// #include "LuxEngine/Threads/ThreadPool.hpp"



namespace lux{
// namespace lux{
    void start() {
        Thread coreThr(lux::core::run, L{ false });
        coreThr.detach();
        while(!core::initialized) lux::thr::self::yield();
    }
    // void init() {
    //     Thread coreThr(lux::core::run, L{ false });
    //     coreThr.detach();
    //     while(!core::initialized) lux::thr::self::yield();
    // }
}