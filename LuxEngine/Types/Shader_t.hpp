#include "LuxEngine/Types/VPointer.hpp"



namespace lux{
    struct Shader_b{
        vram::ptr<char> vdata;  //Gpu data
         ram::ptr<char>  data;  //Local data copy
        uint32         bind;    //GLSL binding point
    };
}