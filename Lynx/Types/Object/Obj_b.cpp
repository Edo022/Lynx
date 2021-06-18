#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"






namespace lnx::obj{
	std::atomic<uint64> obj_bb::Common::lastID = 0;	//#LLID LOS000 initialize the last object ID //FIXME REMOVE or do something idk



	//Only define in debug mode for non debug objects
	#ifdef LNX_DEBUG
	void obj2::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		// if(!Obj_bb::render.isDbgObj && debugBorder) {
		// 	debugBorder->data._data.ffp() = vMinLim;
		// }
	}
	void obj2::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		// if(!Obj_bb::render.isDbgObj && debugBorder) {
		// 	debugBorder->data._data.fsp() = vMaxLim;
		// }
	}
	#endif




	void obj2::onSpawn(Window& pWindow) {
		obj_bb::onSpawn(pWindow);

		// //If in debug mode, spawn debug border
		// #ifdef LNX_DEBUG
        // 	if(!Obj2_bb::render.isDbgObj && !debugBorder) {
		// 		debugBorder = new Border2();
		// 		pWindow.qSpawn(debugBorder);
		// 		// pWindow.requests_m.lock(); //! already managed in draw function
		// 			// debugBorder->render.isDbgObj = true;
		// 			// debugBorder->render.updates = debugBorder->render.updates | obj::UpdateBits::eSpawn;
		// 			// core::render::recSpawn(debugBorder, pWindow);
		// 		// pWindow.requests_m.unlock(); //! already managed in draw function
		// 	}
		// #endif

		//Set callbacks of overwritten inputs
		if(doesRedefine(*this, &MouseCallbacks_b::onClick))pWindow.icQueues.onClick.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onEnter))pWindow.icQueues.onEnter.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onExit ))pWindow.icQueues.onExit .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onMove ))pWindow.icQueues.onMove .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onAxis ))pWindow.icQueues.onAxis .add(this);
    }




	void obj2::setChildLimits(const uint32 vChildIndex) const {
		dbg::checkParam(vChildIndex > this->children.count() - 1, "vChildIndex", "Invalid index");
		this->children[vChildIndex]->setMinLim(minLim);
		this->children[vChildIndex]->setMaxLim(maxLim);
	}


	void obj2::onLimit() {
		obj_bb::onLimit();
		if(parent) parent->setChildLimits(obj_bb::common.childIndex);
		// if(!render.isDbgObj && debugBorder) debugBorder->queue(obj::UpdateBits::eLimit);
	}

	void obj2::onUpdateg(vk::CommandBuffer pCB) {
		pCB.updateBuffer(
			obj_bb::getShVData().cell->csc.buffer,
			obj_bb::getShVData().cell->localOffset,
			obj_bb::getShVData().cell->cellSize,
			(void*)obj_bb::getShData()
		);
		// if(!render.isDbgObj && debugBorder) debugBorder->queue(obj::UpdateBits::eUpdateg);
	}
}