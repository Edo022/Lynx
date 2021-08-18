#pragma once
#define LNX_H_VOLUME
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/3D/Octree.hpp"
#include "Lynx/shaders/3DTest.hpp"
#include "Lynx/Types/Containers/RaArray.hpp"
#include "Lynx/Types/VPointer.hpp"




namespace lnx{
    struct Point{
        f32v3 pos;  //position
        f32 r;      //radius
        f32v4 col;  //color
    };

    struct Volume : public obj::obj3_b{
    private:
        shd::_3DTest data;
        RaArray<Octree> hbx_nodes; //TODO ?? remove?

    public:
        Octree hbx;                                 //Hitbox
        vram::ptr<Point, eVRam, eStorage> points;   //GPU data
        //TODO Generation data

        Volume(const f32v3 vPos, const f32v3 vRot, const RtArray<Point>& vPoints) {
            obj3_b::pos = vPos;
            obj3_b::rot = vRot;
            obj3_b::scl = { 1, 1, 1 };

            for(auto i : vPoints) hbx_nodes.add({
                i.pos, i.r, 0, { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }, nullptr, {}
            });


            // data._data
        }
    };
}
