
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/Shader_t.hpp"



namespace lnx::shd{
	struct Border2 : public Shader_b {
		static Shader_b::Layout layout;
		static uint32 pipelineIndex;


		struct _outcol_t : public ShaderElm_b<Storage> {
			_outcol_t() {
				ShaderElm_b::bind = 0;
			}
		} _outcol;


		struct _wsize_t : public ShaderElm_b<Storage> {
			_wsize_t() {
				ShaderElm_b::bind = 1;
			}
		} _wsize;


		struct _zbuff_t : public ShaderElm_b<Storage> {
			_zbuff_t() {
				ShaderElm_b::bind = 2;
			}
		} _zbuff;


		struct objData__t : public ShaderElm_b<Uniform> {
			objData__t() {
				ShaderElm_b::vdata.realloc(256);
				ShaderElm_b::data.realloc(256);
				ShaderElm_b::bind = 3;
			}
			//Position of the top-left corner
			alwaysInline f32v2& ffp() { return *(f32v2*)ShaderElm_b::data; }
			//Position of the bottom-right corner
			alwaysInline f32v2& fsp() { return *(f32v2*)(ShaderElm_b::data + 8); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(ShaderElm_b::data + 16); }
		} objData_;


		void create(vram::ptr<f32v4, VRam, Storage> pOutcol, vram::ptr<u32v2, VRam, Storage> pWsize, vram::ptr<u32, VRam, Storage> pZbuff, const u32v3 vGroupCount, Window& pWindow);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, Window& pWindow);
		void updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow);
		void destroy();
	};
}//TODO remove local data in external bindings