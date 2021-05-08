#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"


namespace lnx::obj{
	//In debug mode, setMinLim and setMaxLim functions of non debug objects and update the debug border
	#ifdef LNX_DEBUG
	template<class chType> void Obj2_b<chType>::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!Obj_b::debug && debugBorder) {
			debugBorder->data._data.ffp() = vMinLim;
			debugBorder->qHierarchy();
		}
	}
	template<class chType> void Obj2_b<chType>::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!Obj_b::debug && debugBorder) {
			debugBorder->data._data.fsp() = vMaxLim;
			debugBorder->qHierarchy();
		}
	}
	#endif




	template<class chType> void Obj2_b<chType>::onSpawn(Window& pWindow) {
        Obj_b::render.parentWindow = &pWindow;
        for(u32 i = 0; i < Obj2_b<chType>::children.count(); ++i){
            if(Obj2_b<chType>::children.isValid(i)) Obj2_b<chType>::children[i]->onSpawn(pWindow);
        }
		#ifdef LNX_DEBUG
        	if(!Obj_b::debug) {
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




	template<class chType> void Obj2_b<chType>::setChildLimits(const uint32 vChildIndex) const {
		dbg::checkParam(vChildIndex > Obj2_b<chType>::children.count() - 1, "vChildIndex", "Invalid index");
		Obj2_b<chType>::children[vChildIndex]->setMinLim(minLim);
		Obj2_b<chType>::children[vChildIndex]->setMaxLim(maxLim);
	}




	template<class chType> void Obj2_b<chType>::qHierarchy() {
		for(u32 i = 0; i < Obj2_b<chType>::children.count(); i++) if(Obj2_b<chType>::children.isValid(i)) {
			setChildLimits(i);
			//TODO add  recalculateCoords() in all objects
			Obj2_b<chType>::children[i]->recalculateCoords();
			Obj2_b<chType>::children[i]->qHierarchy();
		}
		qSelf();
	}
}