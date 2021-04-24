//This file is only compiled in application builds by lynxg++
#include "Lynx/macros.hpp"
#include "Lynx/Core/AutoInit.hpp"

LnxAutoInit(AA){
    printf("DEBUG_1\n");
    fflush(stdout);
}

neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time