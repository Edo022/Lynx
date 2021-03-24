
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct Border2D{
		struct colorOutput_ : public Shader_b<Storage> {
			colorOutput_() {
				Shader_b::bind = 0;
			}
			alwaysInline f32v4& colorOutput() { return *(f32v4*)Shader_b::data; }
		};


		struct windowSize_ : public Shader_b<Storage> {
			windowSize_() {
				Shader_b::vdata.realloc(8);
				Shader_b::data.realloc(8);
				Shader_b::bind = 1;
			}
			alwaysInline u32v2& window() { return *(u32v2*)Shader_b::data; }
		};


		struct zBuffer_ : public Shader_b<Storage> {
			zBuffer_() {
				Shader_b::bind = 2;
			}
			alwaysInline u32& zBuffer() { return *(u32*)Shader_b::data; }
		};


		struct objData_ : public Shader_b<Uniform> {
			objData_() {
				Shader_b::vdata.realloc(20);
				Shader_b::data.realloc(20);
				Shader_b::bind = 3;
			}
			//Position of the top-left corner
			alwaysInline f32v2& ffp() { return *(f32v2*)Shader_b::data; }
			//Position of the bottom-right corner
			alwaysInline f32v2& fsp() { return *(f32v2*)(Shader_b::data + 8); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(Shader_b::data + 16); }
		};
	};
}