//This file is only compiled in application builds by the Build script
#include "Lynx/Utils.hpp"
#include "Lynx/Core/Init.hpp"


neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time