#pragma once
#define LUX_H_INPUT_SURFACE
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"



namespace lux::obj{
    virtual struct InputSurface : public Obj2_b{
        virtual void onClick(const f32v2 vPos, /*FIXME*/ vButton) = 0;
        virtual void onHover(const f32v2 vPos) = 0;
        virtual void onEnter(const f32v2 vPos) = 0;
        virtual void onExit(const f32v2 vPos) = 0;
        virtual void onAxis(const i32 vAxis) = 0;
    };
}
