
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader_t.hpp"



namespace lux::shd{
	struct Border2D : public Shader_b {
		static Shader_b::Layout layout;


		struct colorOutput__t : public ShaderElm_b<Storage> {
			colorOutput__t() {
				ShaderElm_b::bind = 0;
			}
		} colorOutput_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::bind = 1;
			}
		} windowSize_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
		} zBuffer_;


		struct objData__t : public ShaderElm_b<Uniform> {
			objData__t() {
				ShaderElm_b::vdata.realloc(96);
				ShaderElm_b::data.realloc(96);
				ShaderElm_b::bind = 3;
			}
			//Position of the top-left corner
			alwaysInline f32v2& ffp() { return *(f32v2*)ShaderElm_b::data; }
			//Position of the bottom-right corner
			alwaysInline f32v2& fsp() { return *(f32v2*)(ShaderElm_b::data + 8); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(ShaderElm_b::data + 16); }
		} objData_;


		void create(vram::ptr<f32v4, VRam, Storage> pColorOutput, vram::ptr<u32v2, VRam, Storage> pWindow, vram::ptr<u32, VRam, Storage> pZBuffer){
			colorOutput_.vdata = (vram::ptr<char, VRam, Storage>)pColorOutput;
			windowSize_.vdata = (vram::ptr<char, VRam, Storage>)pWindow;
			zBuffer_.vdata = (vram::ptr<char, VRam, Storage>)pZBuffer;
		}


		void createDescriptorSets();
		void createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow);
		void updateCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow);
	};
}//TODO remove local data in external bindings