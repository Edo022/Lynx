#pragma once
#define LUX_H_VOLUME
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include "LuxEngine/Types/LuxObject/3D/Octree.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"



struct Point{
    f32v3 pos; //position
    f r; //radius
    f32v4 col; //color
};

struct Volume : public Base3{
    Octree hbx; //Hitbox
    vram::ptr<Point> data; //GPU data
    RaArray<Point> ldata; //Local copy of the GPU data //TODO REMOVE
    //TODO Generation data
};
