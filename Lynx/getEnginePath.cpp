//This file is only compiled in application builds by lynxg++
#define LNX_H_GET_ENGINE_PATH
#include "Lynx/macros.hpp"
#include "Lynx/Core/AutoInit.hpp"


neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time