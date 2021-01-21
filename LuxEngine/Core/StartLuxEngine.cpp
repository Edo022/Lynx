#define LUX_NH_START_CORE
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Threads/Thread.hpp"
#include "LuxEngine/Threads/ThreadPool.hpp"



namespace lux::core{
    luxAutoInit(LUX_NH_START_CORE) {
        thread coreThr(lux::core::run, lux::HdCtArray{ false });
        thread thrPool(lux::thr::__lp_thr_mng);
        coreThr.detach();
        thrPool.detach();
        while(!initialized) lux::thr::self::yield();
    }
}