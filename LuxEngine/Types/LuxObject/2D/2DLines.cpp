
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DLines.hpp"




namespace lux::obj{
	//TODO calculate offset and cell count at runtime
	void Line2D::init(Window& pWindow) {
		Obj2_b::init(pWindow); //Initialize all the children


		// core::c::shaders::newShader( //BUG >OVER >OVER >OVER >IN
		// 	RtArray<vram::Alloc_b<int32>>{ //FIXME REMOVE TYPE INFORMATION FROM BASE IF POSSIBLE IDK
		// 		(vram::Alloc_b<int32>)pWindow.fOut_g,
		// 		(vram::Alloc_b<int32>)pWindow.wSize_g,
		// 		(vram::Alloc_b<int32>)pWindow.zBuff_g,
		// 		(vram::Alloc_b<int32>)(data.lineData_.vdata)
		// 	},
		// 	render.shaderLayout, 4, 1, 1,
		// 	pWindow
		// );

		pWindow.addShaderFence.lock();
			data.create(pWindow.fOut_g, pWindow.wSize_g, pWindow.zBuff_g);
			data.createDescriptorSets(render.shaderLayout, pWindow); 				//TODO fix render.shaderLayout
			data.createCommandBuffers(render.shaderLayout, 4, 1, 1, pWindow);		//FIXME 4 group size? wtf?

			pWindow.swp.shadersCBs.add(data.commandBuffers[0]);
		pWindow.addShaderFence.unlock();
	}
}

//FIXME USE POLYMORPHISM. USE SHADER_B IN BASE OBJECT
//FIXME PROBABLY. idk