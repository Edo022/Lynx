#pragma once
#define LUX_H_OBJECT1
#include "Lynx/Types/LuxObject/Obj_b.hpp"




namespace lux::obj{
    //Base class for 1D objects in 1D space
    struct Obj1_b : public Base {
        float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
        float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
        float32 scl{ 0 };				//Scale of the object

        ////TODO add absolute pixel position and scale
        //Obj2_b* parent{ nullptr };						//Parent of the object
        //lux::Map<Obj2_b*, uint32> children;				//Children of the object
        //void setChildLimits(const uint32 vChildIndex) final {
        //	children[vChildIndex]->minLim = minLim;
        //	children[vChildIndex]->maxLim = maxLim;
        //}
        //vec2f32 minLim{ 0, 0 };							//The limit of the object render. It depends on the parent of the object and its properties
        //vec2f32 maxLim{ 0, 0 };							//The limit of the object render. It depends on the parent of the object and its properties
    };
}