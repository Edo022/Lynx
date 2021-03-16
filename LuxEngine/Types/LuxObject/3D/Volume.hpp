#pragma once
#define LUX_H_VOLUME
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include "LuxEngine/Types/LuxObject/3D/Octree.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"




namespace lux{
    struct Point{
        f32v3 pos;  //position
        f32 r;      //radius
        f32v4 col;  //color
    };

    struct Volume : public obj::Base3D{
        Octree hbx;                             //Hitbox
        vram::ptr<Point, VRam, Storage> data;   //GPU data
        RaArray<Point> ldata;                   //Local copy of the GPU data //TODO REMOVE
        //TODO Generation data
    };
}
