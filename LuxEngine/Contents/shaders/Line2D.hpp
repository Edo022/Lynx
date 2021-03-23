
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct Line2D{
		struct colorOutput_ : public Shader_b {
			colorOutput_() { Shader_b::data.realloc(16); }
			f32v4& []colorOutput() { return *(f32v4*)Shader_b::data; }
		};


		struct windowSize_ : public Shader_b {
			windowSize_() { Shader_b::data.realloc(8); }
			u32& width() { return *(u32*)Shader_b::data; }
			u32& height() { return *(u32*)(Shader_b::data + 4); }
		};


		struct zBuffer_ : public Shader_b {
			zBuffer_() { Shader_b::data.realloc(4); }
			u32& []zBuffer() { return *(u32*)Shader_b::data; }
		};


		struct lineData_ : public Shader_b {
			lineData_() { Shader_b::data.realloc(60); }
			//comment
			//0
			f32v2& fp0() { return *(f32v2*)Shader_b::data; }
			/*test
			    */
			//8
			f32v2& fp1() { return *(f32v2*)(Shader_b::data + 8); }
			//16
			f32v4& col0() { return *(f32v4*)(Shader_b::data + 16); }
			//32
			f32v4& col1() { return *(f32v4*)(Shader_b::data + 32); }
			//48
			f32& wd0() { return *(f32*)(Shader_b::data + 48); }
			//52
			f32& wd1() { return *(f32*)(Shader_b::data + 52); }
			//56
			u32& ID() { return *(u32*)(Shader_b::data + 56); }
		};
	};
}