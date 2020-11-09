#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.h"

#define LUX_NH_START_CORE
namespace lux::core{
    AutoInit(LUX_NH_START_CORE){
    	lux::core::preInit( );
        //TODO use lux threads instead of std threads
    	std::thread __t__(lux::core::init, false);
        __t__.detach();
        while(!initialized) sleep(10);
    }
}