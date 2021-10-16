#pragma once
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/3D/Octree.hpp"
#include "Generated/Shaders/Volume.hpp"
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
        shd::gsi::Volume data;
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


            data.data.lCol() = {1,0,0,1};
            data.data.lPos() = {0,0,0};
            data.data.lR() = 1;
        }

        //TODO specify that this function must be overridden in order to make things work
		virtual void onSpawn(core::RenderCore& pRenderCore) override;

        //TODO specify that those functions must be overridden in order to make things work
		virtual ram::ptr<char> getShData() override { return data.data.data; }
		virtual vram::Alloc_b<char> getShVData() override { return data.data.vdata; }
    };
}
