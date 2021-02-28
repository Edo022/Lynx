//This file is only compiled in application builds


#include "LuxEngine/macros.hpp"

neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time