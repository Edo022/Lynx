
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/Shader_t.hpp"



namespace lnx::shd{
	struct Line2 : public Shader_b {
		static Shader_b::Layout layout;
		static uint32 pipelineIndex;


		struct _outcol_t : public ShaderElm_b<eStorage> {
			_outcol_t() {
				ShaderElm_b::bind = 0;
			}
		} _outcol;


		struct _wsize_t : public ShaderElm_b<eStorage> {
			_wsize_t() {
				ShaderElm_b::bind = 1;
			}
		} _wsize;


		struct _zbuff_t : public ShaderElm_b<eStorage> {
			_zbuff_t() {
				ShaderElm_b::bind = 2;
			}
		} _zbuff;


		struct _data_t : public ShaderElm_b<eUniform> {
			_data_t() {
				ShaderElm_b::vdata.realloc(256);
				ShaderElm_b::data.realloc(256);
				ShaderElm_b::bind = 3;
			}
			//Position of the first point
			alwaysInline f32v2& fp0() { return *(f32v2*)ShaderElm_b::data; }
			//Position of the second point
			alwaysInline f32v2& fp1() { return *(f32v2*)(ShaderElm_b::data + 8); }
			//Color of the first point
			alwaysInline f32v4& col0() { return *(f32v4*)(ShaderElm_b::data + 16); }
			//Color of the second point
			alwaysInline f32v4& col1() { return *(f32v4*)(ShaderElm_b::data + 32); }
			//Width of the first point
			alwaysInline f32& wd0() { return *(f32*)(ShaderElm_b::data + 48); }
			//Width of the second point
			alwaysInline f32& wd1() { return *(f32*)(ShaderElm_b::data + 52); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(ShaderElm_b::data + 56); }
		} _data;


		void create(vram::ptr<f32v4, eVRam, eStorage> pOutcol, vram::ptr<u32v2, eVRam, eStorage> pWsize, vram::ptr<u32, eVRam, eStorage> pZbuff, const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void destroy();
	};
}//TODO remove local data in external bindings