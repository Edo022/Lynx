#pragma once
#define LUX_H_INPUT_SURFACE
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"



namespace lux::obj{
    virtual struct InputSurface : public Obj2_b{
        virtual void onClick();
        virtual void onHover();
        virtual void onEnter();
        virtual void onExit();
        virtual void onAxis();
    };
}
