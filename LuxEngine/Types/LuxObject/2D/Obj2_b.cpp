#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"



namespace lux::obj{
	//In debug mode, setMinLim and setMaxLim functions of non debug objects and update the debug border
	#ifdef LUX_DEBUG
	void Obj2_b::setMinLim(f32v2 vMinLim) {
		minLim = vMinLim;
		if(!debug) {
			if(!debugBorder) {
				debugBorder = new Border2D();
				debugBorder->debug = true;
			}
			// *debugBorder->fp = vMinLim;
			debugBorder->data.objData_.ffp() = vMinLim;
			debugBorder->update();
		}
	}
	void Obj2_b::setMaxLim(f32v2 vMaxLim) {
		maxLim = vMaxLim;
		if(!debug) {
			if(!debugBorder) {
				debugBorder = new Border2D();
				debugBorder->debug = true;
			}
			// *debugBorder->sp = vMaxLim;
			debugBorder->data.objData_.fsp() = vMaxLim;
			debugBorder->update();
		}
	}
	#endif
}