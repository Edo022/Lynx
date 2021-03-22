
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd::Line2D{
	struct colorOutput_ : public Shader_b {
		f32v4& []colorOutput() { return *(f32v4*)Shader_b::data; }
	};

	struct windowSize_ : public Shader_b {
		u32& width() { return *(u32*)Shader_b::data; } u32& height() { return *(u32*)(Shader_b::data +4); }
	};

	struct zBuffer_ : public Shader_b {
		u32& []zBuffer() { return *(u32*)Shader_b::data; }
	};

	struct lineData_ : public Shader_b {
		f32v2& _vec2() { return *(f32v2*)Shader_b::data; }     //0
		f32v2& fp1() { return *(f32v2*)(Shader_b::data +8); }       //8
		    //TEST
		f32v4& col0() { return *(f32v4*)(Shader_b::data +16); }       //16
		    /*TEST*/f32v4& col1() { return *(f32v4*)(Shader_b::data +32); }       //32
		    f32& wd0() { return *(f32*)(Shader_b::data +48); }      //48
		    /*
		TEST
		    */
		    f32& floatname() { return *(f32*)(Shader_b::data +52); }        //52
		    /*TEST  */u32& ID() { return *(u32*)(Shader_b::data +56); }     //56
	};
}