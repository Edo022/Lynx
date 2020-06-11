#pragma once


#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include <vulkan/vulkan.h>



struct ObjectGeometry {
	LuxMap<int32> vertices;		//_ x0 y0 z0 x1 y1 z1 x2 y2 ...
	LuxMap<int32> triangles;	//4.3 billion vetices are enought, right?
};

