
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct Line2D{
		struct colorOutput_ : public Shader_b<Storage> {
			colorOutput_() {
				Shader_b::vdata.realloc(16);
				Shader_b::data.realloc(16);
				Shader_b::bind = 0;
			}
			alwaysInline f32v4& []colorOutput() { return *(f32v4*)Shader_b::data; }
		};


		struct windowSize_ : public Shader_b<Storage> {
			windowSize_() {
				Shader_b::vdata.realloc(8);
				Shader_b::data.realloc(8);
				Shader_b::bind = 1;
			}
			alwaysInline u32& width() { return *(u32*)Shader_b::data; }
			alwaysInline u32& height() { return *(u32*)(Shader_b::data + 4); }
		};


		struct zBuffer_ : public Shader_b<Storage> {
			zBuffer_() {
				Shader_b::vdata.realloc(4);
				Shader_b::data.realloc(4);
				Shader_b::bind = 2;
			}
			alwaysInline u32& []zBuffer() { return *(u32*)Shader_b::data; }
		};


		struct lineData_ : public Shader_b<Uniform> {
			lineData_() {
				Shader_b::vdata.realloc(60);
				Shader_b::data.realloc(60);
				Shader_b::bind = 3;
			}
			//Position of the first point
			alwaysInline f32v2& fp0() { return *(f32v2*)Shader_b::data; }
			//Position of the second point
			alwaysInline f32v2& fp1() { return *(f32v2*)(Shader_b::data + 8); }
			//Color of the first point
			alwaysInline f32v4& col0() { return *(f32v4*)(Shader_b::data + 16); }
			//Color of the second point
			alwaysInline f32v4& col1() { return *(f32v4*)(Shader_b::data + 32); }
			//Width of the first point
			alwaysInline f32& wd0() { return *(f32*)(Shader_b::data + 48); }
			//Width of the second point
			alwaysInline f32& wd1() { return *(f32*)(Shader_b::data + 52); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(Shader_b::data + 56); }
		};
	};
}