
//####################################################################################
// This file was generated by Lynx/shaders/GlslToCpp.py
// Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/ShaderInterface_t.hpp"



namespace lnx::shd::gsi{
    struct Border2 : public ShaderInterface_b {
        static ShaderInterface_b::Layout layout;
        static uint32 pipelineIndex;


		struct l_outcol : public ShaderElm_b<eStorage> {
			alwaysInline l_outcol(const bool vExt) : ShaderElm_b() {}
			inline l_outcol() : ShaderElm_b(0) {}
			inline l_outcol(const l_outcol& pOutcol) {
				ShaderElm_b:: data = pOutcol. data;
				ShaderElm_b::vdata = pOutcol.vdata;
			}
			inline l_outcol& operator=(const l_outcol& pOutcol) {
				ShaderElm_b:: data = pOutcol. data;
				ShaderElm_b::vdata = pOutcol.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_outcol(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			f32v4& outcol = *(f32v4*)(ShaderElm_b::data + 0);
			uint64 outcol_tmp_size = 0;
		};
		l_outcol outcol{ true };


		struct l_wsize : public ShaderElm_b<eStorage> {
			alwaysInline l_wsize(const bool vExt) : ShaderElm_b() {}
			inline l_wsize() : ShaderElm_b(256) {}
			inline l_wsize(const l_wsize& pWsize) {
				ShaderElm_b:: data = pWsize. data;
				ShaderElm_b::vdata = pWsize.vdata;
			}
			inline l_wsize& operator=(const l_wsize& pWsize) {
				ShaderElm_b:: data = pWsize. data;
				ShaderElm_b::vdata = pWsize.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_wsize(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			u32v2& wsize = *(u32v2*)(ShaderElm_b::data + 0);
		};
		l_wsize wsize{ true };


		struct l_zbuff : public ShaderElm_b<eStorage> {
			alwaysInline l_zbuff(const bool vExt) : ShaderElm_b() {}
			inline l_zbuff() : ShaderElm_b(0) {}
			inline l_zbuff(const l_zbuff& pZbuff) {
				ShaderElm_b:: data = pZbuff. data;
				ShaderElm_b::vdata = pZbuff.vdata;
			}
			inline l_zbuff& operator=(const l_zbuff& pZbuff) {
				ShaderElm_b:: data = pZbuff. data;
				ShaderElm_b::vdata = pZbuff.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_zbuff(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			u32& zbuff = *(u32*)(ShaderElm_b::data + 0);
			uint64 zbuff_tmp_size = 0;
		};
		l_zbuff zbuff{ true };


		struct l__data : public ShaderElm_b<eUniform> {
			alwaysInline l__data(const bool vExt) : ShaderElm_b() {}
			inline l__data() : ShaderElm_b(256) {}
			inline l__data(const l__data& p_data) {
				ShaderElm_b:: data = p_data. data;
				ShaderElm_b::vdata = p_data.vdata;
			}
			inline l__data& operator=(const l__data& p_data) {
				ShaderElm_b:: data = p_data. data;
				ShaderElm_b::vdata = p_data.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l__data(const vram::ptr<auto, eVRam, eUniform>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eUniform>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eUniform>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eUniform>)pVPtr;
				return *this;
			}
			f32v2& ffp = *(f32v2*)(ShaderElm_b::data + 0);
			f32v2& fsp = *(f32v2*)(ShaderElm_b::data + 8);
			u32& ID = *(u32*)(ShaderElm_b::data + 16);
		};
		l__data _data;


		void spawn(
			const l_outcol& pOutcol,
			const l_wsize& pWsize,
			const l_zbuff& pZbuff,
			const u32v3 vGroupCount, core::RenderCore& pRenderCore
		);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void destroy();
    };
}