
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/2D/Line2.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"

//TODO DRAW OBJECT-RENDERCORE INTERACTION GRAPH
//TODO object-RenderCore. similar to client-server





namespace lnx::obj{
	void Line2::onSpawn(Window& pWindow) {//FIXME PASS RENDER CORE
		obj2_b::onSpawn(pWindow); //Initialize all the children
		data.create(pWindow.renderCore.fOut_g, pWindow.renderCore.wSize_g, pWindow.renderCore.zBuff_g, { 512u, 1u, 1u }, pWindow); //FIXME CALCULATE GROUP SIZE AT RUNTIME
	}

	void Line2::onLimit() {
		obj2_b::onLimit();
		dbg::checkCond(w && thr::self::thr() != w->renderCore.t.thr, "This function can only be called by the render thread.");
		data._data.fp0() = _fp0 * adist(this->minLim, this->maxLim) + this->minLim;
		data._data.fp1() = _fp1 * adist(this->minLim, this->maxLim) + this->minLim;
	}
}