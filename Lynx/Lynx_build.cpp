//Link order
//Including cpps like this also improves g++ compilation time
//Do NOT #include this file


#ifndef __INTELLISENSE__
    #include "Lynx/System/SystemInfo.cpp"
    #include "Lynx/Tests/StructureInit.cpp"

    #include "Lynx/Core/Memory/Ram/Ram.cpp"
    #include "Lynx/Core/Memory/Ram/Cell_t.cpp"

    #include "Lynx/System/System.cpp"

    #include "Lynx/Core/Devices.cpp"
    #include "Lynx/Core/Memory/VRam/VRam.cpp"
    #include "Lynx/Core/Memory/VRam/VCell_t.cpp"

    #include "Lynx/Core/Render/GCommands.cpp"
    #include "Lynx/Core/Render/Shaders/Shader.cpp"
    #include "Lynx/Core/Render/Window/Window.cpp" //FIXME
    #include "Lynx/Core/Render/Window/Swapchain.cpp"
    #include "Lynx/Core/Core.cpp"


    #include "Lynx/Threads/ThreadPool.cpp"
        //#include "Lynx/Math/Trigonometry/GoniometricFunctions.cpp"

    #include "Lynx/Types/Object/Obj_b.cpp"
    #include "Lynx/Types/Object/1D/Obj1_b.cpp"
    #include "Lynx/Types/Object/2D/Obj2_b.cpp"
    #include "Lynx/Types/Object/3D/Obj3_b.cpp"

    #include "Lynx/Types/Object/2D/Line2.cpp"
    #include "Lynx/Types/Object/2D/Border2.cpp"
    #include "Lynx/Types/Object/2D/RenderSpace2.cpp"

    //TODO AUTOMATIZE
    #include "Lynx/shaders/Line2.cpp"
    #include "Lynx/shaders/Border2.cpp"
    #include "Lynx/shaders/FloatToIntBuffer.cpp"


    #include "Lynx/Core/Render/Buffers.cpp"
    #include "Lynx/Core/Render/Render.cpp"

    #include "Lynx/Core/Input/Input.cpp"
    #include "Lynx/Core/Input/InputState.cpp"
#endif