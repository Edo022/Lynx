#pragma once
#define LUX_H_VOLUME
#include "LuxEngine/Types/LuxObject/Obj_b.hpp"
#include "LuxEngine/Types/LuxObject/3D/Octree.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"




namespace lux{
    struct Point{
        f32v3 pos;  //position
        f32 r;      //radius
        f32v4 col;  //color
    };

    struct Volume : public obj::Obj3_b{
    private:
        RaArray<Octree> hbx_nodes;

    public:
        Octree hbx;                             //Hitbox
        vram::ptr<Point, VRam, Storage> data;   //GPU data
        //TODO Generation data

        Volume(f32v3 vPos, f32v2 vRot, RtArray<Point>& vData) {
            Obj3_b::pos = vPos;
            Obj3_b::rot = vRot;
            Obj3_b::scl = { 1, 1, 1 };

            for(auto i : vData) hbx_nodes.add({
                i.pos, i.r, 0, { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }, nullptr, {}
            });
        }
    };
}
