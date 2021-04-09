#pragma once
#define LNX_H_INPUT_SURFACE
#include "Lynx/Types/Object/2D/Obj2_b.hpp"



namespace lnx::obj{
    struct InputSurface : public Obj2_b{
        virtual void onClick(const f32v2 vPos, int vButton) = 0;
        virtual void onHover(const f32v2 vPos) = 0;
        virtual void onEnter(const f32v2 vPos) = 0;
        virtual void onExit(const f32v2 vPos) = 0;
        virtual void onAxis(const i32 vAxis) = 0;
    };
}
