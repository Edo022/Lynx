#pragma once
#define LUX_H_SHADER_T
#include "LuxEngine/Types/VPointer.hpp"



namespace lux{
	template<bufferType buft> struct Shader_b {
		vram::ptr<char, VRam, buft> vdata;	//Gpu data
		ram::ptr<char>              data;	//Local data copy
		uint32                      bind;	//GLSL binding point
	};
}