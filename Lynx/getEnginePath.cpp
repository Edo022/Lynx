//This file is only compiled in application builds by lynxg++
#include "Lynx/macros.hpp"

neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time