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
			*debugBorder->fp = vMinLim;
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
			*debugBorder->sp = vMaxLim;
			debugBorder->update();
		}
	}
	#endif
}