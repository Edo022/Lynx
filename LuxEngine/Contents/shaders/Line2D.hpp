
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct Line2D : public Shader_b {
		struct colorOutput__t : public ShaderElm_b<Storage> {
			colorOutput__t() {
				ShaderElm_b::bind = 0;
			}
			alwaysInline f32v4& colorOutput() { return *(f32v4*)ShaderElm_b::data; }
		} colorOutput_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::vdata.realloc(8);
				ShaderElm_b::data.realloc(8);
				ShaderElm_b::bind = 1;
			}
			alwaysInline u32& width() { return *(u32*)ShaderElm_b::data; }
			alwaysInline u32& height() { return *(u32*)(ShaderElm_b::data + 4); }
		} windowSize_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
			alwaysInline u32& zBuffer() { return *(u32*)ShaderElm_b::data; }
		} zBuffer_;


		struct lineData__t : public ShaderElm_b<Uniform> {
			lineData__t() {
				ShaderElm_b::vdata.realloc(96);
				ShaderElm_b::data.realloc(96);
				ShaderElm_b::bind = 3;
			}
			//Position of the first point
			alwaysInline f32v2& fp0() { return *(f32v2*)ShaderElm_b::data; }
			//Position of the second point
			alwaysInline f32v2& fp1() { return *(f32v2*)(ShaderElm_b::data + 8); }
			//Color of the first point
			alwaysInline f32v4& col0() { return *(f32v4*)(ShaderElm_b::data + 16); }
			//Width of the first point
			alwaysInline f32& wd0() { return *(f32*)(ShaderElm_b::data + 32); }
			//Color of the second point
			alwaysInline f32v4& col1() { return *(f32v4*)(ShaderElm_b::data + 48); }
			//Width of the second point
			alwaysInline f32& wd1() { return *(f32*)(ShaderElm_b::data + 64); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(ShaderElm_b::data + 68); }
		} lineData_;


		void create(vram::ptr<f32v4, VRam, Storage> pColorOutput, vram::ptr<u32, VRam, Storage> pZBuffer){

		}
	};
}