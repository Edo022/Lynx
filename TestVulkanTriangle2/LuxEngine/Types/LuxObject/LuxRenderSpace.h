#pragma once

#include "LuxEngine/Types/LuxObject/LuxObject.h"


//A 2D space where to render LuxObject s
//The position of a 2D LuxObject is relative to the size of the renderSpace
//    -1, -1 is the beginning of the render space. +1, +1 is its end
//    Out of view objects will be cropped if the renderSpaces does not allow outOfViewRender
//A renderSpace can also contain other renderSpace s. They'll be rendered like every other object
struct LuxRenderSpace2D : public LuxObject2D_base1 {
	static const LuxObjectType objectType = LUX_OBJECT_TYPE_RENDER_SPACE_2D;
	LuxArray<LuxObject2D_base1> objects;
	bool outOfViewRender = false;
	//TODO
};

//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space