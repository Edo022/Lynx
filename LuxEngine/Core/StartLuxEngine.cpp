#define LUX_NH_START_CORE
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"



namespace lux::core{
    luxAutoInit(LUX_NH_START_CORE){
        lux::thread t(lux::core::run, lux::HdCtArray{ false });
        t.detach();
        while(!initialized) lux::thr::self::yield();
    }
}