//############################################################################################
//!  This file is supposed to be used during the compilation of the engine source files
//!  It allows the engine to be initialized before any user defined global variable
//!  Do not compile or include it. It's done automatically
//############################################################################################


#include <mutex>
#include <deque>
#include <atomic>
#include <Lynx/Core/IncludeVulkan.hpp>
#include <GLFW/glfw3.h>
#include "Lynx/Types/Integers/Integers.hpp"




struct Device;

namespace lnx{
    template<class tType, class tIdxt> struct RaArrayC;
    template<class tType, class tIdxt> struct RtArray;
    template<class tType, std::integral tIdxt> struct RaArray;
    class String;
    struct Thread;

    namespace sys{
        struct CpuInfo;
        struct RamInfo;
        struct GpuInfo;
    }

    namespace ram{
        struct Type_t;
        struct Cell_t;
        template<class tType> struct ptr;
    }

    namespace vram{
        struct Type_t2;
        struct Cell_t2;
    }

    namespace shd{
        struct ShaderInterface_b;
        struct InterfaceLayout;
    }

    namespace thr::_pvt{
        struct Func_b;
    }
}









#include "Generated/.init/SystemInfo.hpp"
#include "Generated/.init/StructureInit.hpp"

// #include "Generated/.init/Ram.hpp"
#include "Generated/.init/Cell_t.hpp"

#include "Generated/.init/System.hpp"

#include "Generated/.init/Devices.hpp"
#include "Generated/.init/VRam.hpp"
#include "Generated/.init/VCell_t.hpp"

// #include "Generated/.init/GCommands.hpp"
#include "Generated/.init/Shader.hpp"
// #include "Generated/.init/Window.hpp" //FIXME
// #include "Generated/.init/Swapchain.hpp"

#include "Generated/.init/Core.hpp"
#include "Generated/.init/ThreadPool.hpp"

// #include "Generated/.init/Buffers.hpp"
#include "Generated/.init/Render.hpp"

// #include "Generated/.init/Input.hpp"
// #include "Generated/.init/InputState.hpp"
