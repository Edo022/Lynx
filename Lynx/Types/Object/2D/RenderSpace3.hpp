#pragma once
////#define LNX_H_RENDER_SPACE_3D
#include "Lynx/Types/Object/Obj_b.hpp"




namespace lnx::obj{
    struct RenderSpace3 : public StructObj2{
        RenderSpace3(){ }

        uint32 addChild(obj3_b* pObject);
        void setChildLimits(const uint32 vChildIndex) const final {} //3D objects have no limits //FIXME idK
    };
}