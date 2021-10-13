//This file is only compiled in application builds by the Build script
////#define LNX_H_GET_ENGINE_PATH
#include "Lynx/Utils.hpp"
#include "Lynx/Core/Init.hpp"


neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time