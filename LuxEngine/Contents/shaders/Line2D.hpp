
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd::Line2D{
	struct colorOutput_ : public Shader_b {
		f32v4& getColorOutput() { return *(f32v4*)Shader_b::data; }
	};

	struct windowSize_ : public Shader_b {
		u32& getWidth() { return *(u32*)Shader_b::data; }u32& getHeight() { return *(u32*)(Shader_b::data +4); }
	};

	struct zBuffer_ : public Shader_b {
		u32& getZBuffer() { return *(u32*)Shader_b::data; }
	};

	struct lineData_ : public Shader_b {
		f32v2& get_vec2() { return *(f32v2*)Shader_b::data; }//0
		f32v2& getFp1() { return *(f32v2*)(Shader_b::data +8); }//8
		//TEST
		//16
		/*TEST*///32
		f32& getWd0() { return *(f32*)(Shader_b::data +16); }//48
		/*
		TEST
		    */f32& getFloatname() { return *(f32*)(Shader_b::data +20); }//52
		/*TEST  */
	};
}