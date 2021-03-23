#include "LuxEngine/Types/VPointer.hpp"



namespace lux{
    struct Shader_b{
        ram::ptr<char> data;    //Local data copy
        uint32         bind;    //GLSL binding point
    };
}