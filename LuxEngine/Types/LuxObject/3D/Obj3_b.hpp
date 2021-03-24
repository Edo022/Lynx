#pragma once
#define LUX_H_OBJECT2
#include "LuxEngine/Types/LuxObject/Obj_b.hpp"




namespace lux::obj{
    //3D object in 3D space
    struct Obj3_b : public Base {
        Obj3_b() { common.objectType = LUX_OBJECT_TYPE_3D__BASE; }

        f32v3 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
        float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
        f32v3 rot{ 0, 0, 0 };			//Rotation of the object
        f32v3 scl{ 0, 0, 0 };			//Scale of the object

        ////TODO add absolute pixel position and scale
        //Obj3_b* parent{ nullptr };						//Parent of the object
        //lux::Map<Obj3_b*, uint32> children;				//Children of the object
        //void setChildLimits(const uint32 vChildIndex) const override {
        //	children[vChildIndex]->minLim = minLim;
        //	children[vChildIndex]->maxLim = maxLim;
        //}
        //vec3f32 minLim{ 0, 0, 0 };						//The limit of the object render. It depends on the parent of the object and its properties
        //vec3f32 maxLim{ 1, 1, 1 };						//The limit of the object render. It depends on the parent of the object and its properties
    };
}