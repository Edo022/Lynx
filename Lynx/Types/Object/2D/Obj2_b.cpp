#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"








namespace lnx::obj{
	//Only define in debug mode for non debug objects
	#ifdef LNX_DEBUG
	void Obj2_bb::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!Obj_bb::render.isDbgObj && debugBorder) {
			debugBorder->data._data.ffp() = vMinLim;
			// debugBorder->qHierarchy();
			debugBorder->queue(obj::UpdateBits::updateg);
		}
	}
	void Obj2_bb::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!Obj_bb::render.isDbgObj && debugBorder) {
			debugBorder->data._data.fsp() = vMaxLim;
			// debugBorder->qHierarchy();
			debugBorder->queue(obj::UpdateBits::updateg);
		}
	}
	#endif




	template<class chType> void Obj2_bt<chType>::onSpawn(Window& pWindow) {
		dbg::checkCond(thr::self::thr() != pWindow.t.thr, "This function can only be called by the render thread.");
		//BUG BEGIN >>
        // Obj_bb::render.parentWindow = &pWindow;                                   //BUG UNCOMMENT
        // for(u32 i = 0; i < this->children.count(); ++i){                          //BUG UNCOMMENT
        //     if(this->children.isValid(i)) this->children[i]->onSpawn(pWindow);    //BUG UNCOMMENT
        // }                                                                         //BUG UNCOMMENT
		//BUG END <<

		#ifdef LNX_DEBUG
        	if(!Obj_bb::render.isDbgObj) {
				debugBorder = new Border2(); //FIXME ONLY SPAWN IF NOT PRESENT
				debugBorder->render.isDbgObj = true;
				debugBorder->onSpawn(pWindow);
			}
		#endif

		if(doesRedefine(*this, &MouseCallbacks_b::onClick))pWindow.icQueues.onClick.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onEnter))pWindow.icQueues.onEnter.add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onExit ))pWindow.icQueues.onExit .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onMove ))pWindow.icQueues.onMove .add(this);
		if(doesRedefine(*this, &MouseCallbacks_b::onAxis ))pWindow.icQueues.onAxis .add(this);
    }




	template<class chType> void Obj2_bt<chType>::setChildLimits(const uint32 vChildIndex) const {
		dbg::checkParam(vChildIndex > Obj2_bt::children.count() - 1, "vChildIndex", "Invalid index");
		Obj2_bt::children[vChildIndex]->setMinLim(minLim);
		Obj2_bt::children[vChildIndex]->setMaxLim(maxLim);
	}


//FIXME UNIFY UPDATE QUEUES

	template<class chType> void Obj2_bt<chType>::qHierarchy() {
		for(u32 i = 0; i < Obj2_bt::children.count(); i++) if(Obj2_bt::children.isValid(i)) { //BUG UNCOMMENT
			setChildLimits(i); //FIXME USE QUEUES                                                                //BUG UNCOMMENT
			//TODO add  recalculateCoords() in all objects                                                       //BUG UNCOMMENT
			// Obj2_bt<chType>::children[i]->recalculateCoords(); //FIXME USE QUEUES
			Obj2_bt::children[i]->queue(UpdateBits::limit);                                              //BUG UNCOMMENT
			// Obj2_bt::children[i]->qHierarchy();                                                          //BUG UNCOMMENT
		}                                                                                                     //BUG UNCOMMENT
		qSelf(); //FIXME REMOVE
		// queue(UpdateBits::updateg);//FIXME ADD OBJECT TYPE FOR MORE DETAILED DEBUG ERRORS
		//BUG^ NOT OVERRIDDEN IN RENDER SPACES AS QSELF WAS
		//BUG^ ADD TEMPORARY FIX
	}
}