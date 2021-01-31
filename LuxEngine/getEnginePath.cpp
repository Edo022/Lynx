//This file is only compiled in application builds


#include "LuxEngine/macros.hpp"
#ifdef __INTELLISENSE__
    #define enginePath "<generated>"
#endif

neverInline const char* getEnginePath() { return enginePath; }
//!^ enginePath is defined in compile time