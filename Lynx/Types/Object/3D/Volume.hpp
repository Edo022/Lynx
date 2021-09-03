#pragma once
#define LNX_H_VOLUME
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/3D/Octree.hpp"
#include "Lynx/shaders/3DTest.gsi.hpp"
#include "Lynx/Types/Containers/RaArray.hpp"
#include "Lynx/Types/VPointer.hpp"




namespace lnx{
    struct Point{
        f32v3 pos;  //position
        f32 r;      //radius
        f32v4 col;  //color
    };

    struct Volume : public obj::RenderObj3{
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

            //FIXME
            for(auto i : vPoints) hbx_nodes.add({
                i.pos, i.r, 0, { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }, nullptr, {}
            });


            data._data.col() = {1,0,0,1};
            data._data.pos() = {0,0,0};
            data._data.r() = 1;
        }

        //TODO specify that this function must be overridden in order to make things work
		virtual void onSpawn(core::RenderCore& pRenderCore) override;

        //TODO specify that those functions must be overridden in order to make things work
		virtual ram::ptr<char> getShData() override { return data._data.data; }
		virtual vram::Alloc_b<char> getShVData() override { return data._data.vdata; }
    };
}
