#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"








namespace lnx::obj{
	//Only define in debug mode for non debug objects
	#ifdef LNX_DEBUG
	void Obj2_bb::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!Obj_bb::render.isDbgObj && debugBorder) {
			debugBorder->data._data.ffp() = vMinLim;
		}
	}
	void Obj2_bb::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!Obj_bb::render.isDbgObj && debugBorder) {
			debugBorder->data._data.fsp() = vMaxLim;
		}
	}
	#endif




	template<class chType> void Obj2_bt<chType>::onSpawn(Window& pWindow) {
		Obj_bt<chType>::onSpawn(pWindow);

		#ifdef LNX_DEBUG
        	if(!Obj_bb::render.isDbgObj) {
				debugBorder = new Border2(); //FIXME ONLY SPAWN IF NOT PRESENT
				debugBorder->render.isDbgObj = true;
				pWindow.qSpawn(debugBorder);
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
}