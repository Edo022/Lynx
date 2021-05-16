#pragma once
#define LNX_H_RENDER_SPACE
#include "Lynx/Types/Object/2D/Obj2_b.hpp"



namespace lnx::obj{
    struct RenderSpace_b : public Obj2_b<2>{
        void qSelf() final override {} //FIXME REMOVE
        void onUpdateg() final override {
            Obj2_b::onUpdateg();
        }
        //FIXME
        virtual      ram::ptr<char> getShData() override { dbg::printError("Unable to call this base function"); return nullptr; }
        virtual vram::Alloc_b<char> getShVData() override{ dbg::printError("Unable to call this base function"); return vram::Alloc_b<char>(); }
    };
}

