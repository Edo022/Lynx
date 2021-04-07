#pragma once
#define LUX_H_INPUT_SURFACE
#include "LuxEngine/Types/LuxObject/ClearObj_b.hpp"



namespace lux::obj{
    struct InputSurface : public ClearObj_b{
        virtual void onClick();
        virtual void onHover();
        virtual void onEnter();
        virtual void onExit();
        virtual void onAxis();
    };
}
