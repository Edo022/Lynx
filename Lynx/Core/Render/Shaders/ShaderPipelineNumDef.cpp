#include "Lynx/Core/Init.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
//! This file has the only purpose of allocating lnx::core::shaders::pipelineNum before the shader interface initializers can use it

namespace lnx::core::shaders{
    _lnx_init_var_value_def((uint32), pipelineNum, lnx::core::shaders){ pVar = 0; }
    //!                           Incremented by shader interface initializers ^
}