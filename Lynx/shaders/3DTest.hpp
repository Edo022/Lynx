
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/Shader_t.hpp"



namespace lnx::shd{
	struct _3DTest : public Shader_b {
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


		struct _data_t : public ShaderElm_b<Uniform> {
			_data_t() {
				ShaderElm_b::bind = 3;
			}
		} _data;


		void create(vram::ptr<f32v4, VRam, Storage> pOutcol, vram::ptr<u32v2, VRam, Storage> pWsize, vram::ptr<f32v4, VRam, Storage> pData, const u32v3 vGroupCount, Window& pWindow);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, Window& pWindow);
		void updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow);
		void destroy();
	};
}//TODO remove local data in external bindings