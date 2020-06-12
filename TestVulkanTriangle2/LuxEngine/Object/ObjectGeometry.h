#pragma once


#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include <vulkan/vulkan.h>



struct ObjectGeometry {
	LuxGpuBuffer vertices;
	LuxGpuBuffer triangles;
	//TODO
};

