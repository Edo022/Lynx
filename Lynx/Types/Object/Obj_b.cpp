#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"





namespace lnx::obj{
	std::atomic<uint64> obj_bb::lastID = 0;	//#LLID LOS000 initialize the last object ID //FIXME REMOVE or do something idk





	void obj_bb::onSpawn(core::RenderCore& pRenderCore){
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








	// 2D objects base class ----------------------------------------------------------------------------------------------------------------------//








	void obj2_b::setChildLimits(const uint32 vChildIndex) const {
		dbg::checkParam(vChildIndex > obj_bb::children.count() - 1, "vChildIndex", "Invalid index");

		//FIXME ADD CHECK IN ADD FUNCTION TO CHECK THAT CHILDREN ARE OBJ2 ONLY
		static_cast<obj2_b*>(obj_bb::children[vChildIndex])->setMinLim(minLim);
		static_cast<obj2_b*>(obj_bb::children[vChildIndex])->setMaxLim(maxLim);
	}


	void obj2_b::onSpawn(core::RenderCore& pRenderCore) {
		obj_bb::onSpawn(pRenderCore);

		//Set callbacks of overwritten inputs
		if(doesRedefine(*this, &MouseCallbacks_b::onClick))pRenderCore.w->icQueues.onClick.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onEnter))pRenderCore.w->icQueues.onEnter.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onExit ))pRenderCore.w->icQueues.onExit .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onMove ))pRenderCore.w->icQueues.onMove .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onAxis ))pRenderCore.w->icQueues.onAxis .add(this);
	}


	void obj2_b::onLimit() {
		obj_bb::onLimit();
		if(parent) parent->setChildLimits(obj_bb::childIndex);
	}
}