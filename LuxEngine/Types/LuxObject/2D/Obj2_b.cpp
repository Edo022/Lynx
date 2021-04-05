#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"


namespace lux::obj{
	//In debug mode, setMinLim and setMaxLim functions of non debug objects and update the debug border
	#ifdef LUX_DEBUG
	void Obj2_b::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!debug && debugBorder) {
			debugBorder->data.objData_.ffp() = vMinLim;
			debugBorder->update();
		}
	}
	void Obj2_b::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!debug && debugBorder) {
			debugBorder->data.objData_.fsp() = vMaxLim;
			debugBorder->update();
		}
	}
	#endif




	Obj2_b::Obj2_b() {
        common.objectType = LUX_OBJECT_TYPE_2D__BASE;
    }



	void Obj2_b::onSpawn(Window& pWindow) {
        render.parentWindow = &pWindow;                         //BUG OVER
        for(u32 i = 0; i < children.count(); ++i){              //BUG OVER
            if(children.isValid(i)) children[i]->onSpawn(pWindow); //BUG >IN >OUT >IN >OUT >IN
        }
        if(!debug) {
			debugBorder = new Border2D();
			debugBorder->debug = true;
			debugBorder->onSpawn(pWindow);
		}
    }
}