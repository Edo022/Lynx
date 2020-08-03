
#include "LuxEngine/Core/LuxCore.h"

namespace lux{
	//This function returns a reference of the engine object
	Engine& getEngine( ){
		static Engine engine_;
		return engine_;
	}
}


