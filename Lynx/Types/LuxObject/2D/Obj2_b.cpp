#include "Lynx/Types/LuxObject/2D/Obj2_b.hpp"
#include "Lynx/Types/LuxObject/2D/Border2.hpp"


namespace lux::obj{
	//In debug mode, setMinLim and setMaxLim functions of non debug objects and update the debug border
	#ifdef LUX_DEBUG
	void Obj2_b::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!debug && debugBorder) {
			debugBorder->data.objData_.ffp() = vMinLim;
			debugBorder->qHierarchy();
		}
	}
	void Obj2_b::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!debug && debugBorder) {
			debugBorder->data.objData_.fsp() = vMaxLim;
			debugBorder->qHierarchy();
		}
	}
	#endif




	void Obj2_b::onSpawn(Window& pWindow) {
        render.parentWindow = &pWindow;                         //BUG OVER
        for(u32 i = 0; i < children.count(); ++i){              //BUG OVER
            if(children.isValid(i)) children[i]->onSpawn(pWindow); //BUG >IN >OUT >IN >OUT >IN
        }
		#ifdef LUX_DEBUG
        	if(!debug) {
				debugBorder = new Border2();
				debugBorder->debug = true;
				debugBorder->onSpawn(pWindow);
			}
		#endif
    }
}