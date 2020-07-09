#pragma once

#include "LuxEngine/Types/LuxObject/LuxObject.h"


namespace lux::obj {
	//A 2D space where to render LuxObject s
	//The position of a 2D LuxObject is relative to the size of the renderSpace
	//    -1, -1 is the beginning of the render space. +1, +1 is its end
	//    Out of view objects will be cropped if the renderSpaces does not allow outOfViewRender
	//A renderSpace can also contain other renderSpace s. They'll be rendered like every other object
	struct LuxRenderSpace2D : public Base2D {
		static const LuxObjectType objectType = LUX_OBJECT_TYPE_RENDER_SPACE_2D;
		LuxMap<const Base2D*> objects;
		bool outOfViewRender = false;


		//This function adds an objects tto the render queue of the render space and assigns it an object ID
		//*   pObject: the object to spawn. It must be a subStruct of Base2D, but not a base struct
		//*   Returns true if the object is spawned correctly, false if the object is invalid or an error occurs
		bool spawnObject(const Base2D* pObject) {
			if (pObject->objectType < 0) return false;
			objects.add(pObject);
			return true;
		}
		//TODO
	};
}
//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space