
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DLines.hpp"




namespace lux::obj{
	//TODO calculate offset and cell count at runtime
	void Line2D::init(Window& pWindow) {
		Base2D::init(pWindow); //Initialize all the children


		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{ //FIXME REMOVE TYPE INFORMATION FROM BASE IF POSSIBLE IDK
				pWindow.fOut_g,
				pWindow.wSize_g,
				pWindow.zBuff_g,
				(vram::Alloc_b<int32>)(render.localData)
			},
			render.shaderLayout, 4, 1, 1,
			pWindow
		);
	}
}
