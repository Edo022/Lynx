#pragma once
#define LNX_H_RENDER_SPACE
#include "Lynx/Types/Object/2D/Obj2_b.hpp"



namespace lnx::obj{
    struct RenderSpace_b : public Obj2_b<2>{
        void onUpdateg(vk::CommandBuffer pCB) final override {
            Obj_bb::onUpdateg(pCB);
        }

        virtual      ram::ptr<char> getShData() override { dbg::printError("Unable to call this base function"); return nullptr; }
        virtual vram::Alloc_b<char> getShVData() override{ dbg::printError("Unable to call this base function"); return vram::Alloc_b<char>(); }
    };
}

