#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"





namespace lnx::obj{
	std::atomic<uint64> obj_bb::lastID = 0;	//#LLID LOS000 initialize the last object ID //FIXME REMOVE or do something idk





	void obj_bb::onSpawn(Window& pWindow){
		dbg::checkCond(w && thr::self::thr() != w->renderCore.t.thr, "This function can only be called by the render thread.");
	}
	void obj_bb::onLimit(){
		dbg::checkCond(w && thr::self::thr() != w->renderCore.t.thr, "This function can only be called by the render thread.");
	}
	void obj_bb::onFlush(vk::CommandBuffer pCB){
		dbg::checkCond(w && thr::self::thr() != w->renderCore.t.thr, "This function can only be called by the render thread.");
	}


	void obj_bb::queue(UpdateBits vUpdates){
		UpdateBits old = updates;						//Save old updates bits
		if(w) { 										//If the object has a binded window
			w->renderCore.requests_m.lock();				//Lock requests mutex
				updates = updates | vUpdates;				//Update updates bits
				if(!old) w->renderCore.requests.add(this);	//If it isn't already in it, add the object to the update queue
			w->renderCore.requests_m.unlock();				//Unlock requests mutex
		}												//If not
		else updates = updates | vUpdates;					//Update updates bits
	}




	//Only define in debug mode for non debug objects
	#ifdef LNX_DEBUG
		void obj2::setMinLim(f32v2 vMinLim) { minLim = vMinLim; }
		void obj2::setMaxLim(f32v2 vMaxLim) { maxLim = vMaxLim; }
	#endif




	void obj2::onSpawn(Window& pWindow) {
		obj_bb::onSpawn(pWindow);

		//Set callbacks of overwritten inputs
		if(doesRedefine(*this, &MouseCallbacks_b::onClick))pWindow.icQueues.onClick.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onEnter))pWindow.icQueues.onEnter.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onExit ))pWindow.icQueues.onExit .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onMove ))pWindow.icQueues.onMove .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onAxis ))pWindow.icQueues.onAxis .add(this);
    }




	void obj2::setChildLimits(const uint32 vChildIndex) const {
		dbg::checkParam(vChildIndex > obj_bb::children.count() - 1, "vChildIndex", "Invalid index");

		//FIXME ADD CHECK IN ADD FUNCTION TO CHECK THAT CHILDREN ARE OBJ2 ONLY
		static_cast<obj2*>(obj_bb::children[vChildIndex])->setMinLim(minLim);
		static_cast<obj2*>(obj_bb::children[vChildIndex])->setMaxLim(maxLim);
	}


	void obj2::onLimit() {
		obj_bb::onLimit();
		if(parent) parent->setChildLimits(obj_bb::childIndex);
	}

	void obj2::onFlush(vk::CommandBuffer pCB) {
		pCB.updateBuffer(
			getShVData().cell->csc.buffer,
			getShVData().cell->localOffset,
			getShVData().cell->cellSize,
			(void*)getShData()
		);
	}
}