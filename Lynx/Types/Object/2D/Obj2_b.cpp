#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"








namespace lnx::obj{
	//Only define in debug mode for non debug objects
	#ifdef LNX_DEBUG
	void Obj2_bb::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!Obj_bb::debug && debugBorder) {
			debugBorder->data._data.ffp() = vMinLim;
			debugBorder->qHierarchy();
		}
	}
	void Obj2_bb::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!Obj_bb::debug && debugBorder) {
			debugBorder->data._data.fsp() = vMaxLim;
			debugBorder->qHierarchy();
		}
	}
	#endif




	template<class chType> void Obj2_bt<chType>::onSpawn(Window& pWindow) {
        Obj_bb::render.parentWindow = &pWindow;
        for(u32 i = 0; i < Obj2_bt<chType>::children.count(); ++i){
            if(Obj2_bt<chType>::children.isValid(i)) Obj2_bt<chType>::children[i]->onSpawn(pWindow);
        }
		#ifdef LNX_DEBUG
        	if(!Obj_bb::debug) {
				debugBorder = new Border2();
				debugBorder->debug = true;
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
		dbg::checkParam(vChildIndex > Obj2_bt<chType>::children.count() - 1, "vChildIndex", "Invalid index");
		Obj2_bt<chType>::children[vChildIndex]->setMinLim(minLim);
		Obj2_bt<chType>::children[vChildIndex]->setMaxLim(maxLim);
	}




	template<class chType> void Obj2_bt<chType>::qHierarchy() {
		for(u32 i = 0; i < Obj2_bt<chType>::children.count(); i++) if(Obj2_bt<chType>::children.isValid(i)) {
			setChildLimits(i);
			//TODO add  recalculateCoords() in all objects
			Obj2_bt<chType>::children[i]->recalculateCoords();
			Obj2_bt<chType>::children[i]->qHierarchy();
		}
		qSelf();
	}
}