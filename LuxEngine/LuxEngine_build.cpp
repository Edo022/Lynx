//Link order
//Including cpps like this also improves g++ compilation time
//Do NOT #include this file


#ifndef __INTELLISENSE__
    #include "LuxEngine/System/SystemInfo.cpp"
    #include "LuxEngine/Tests/StructureInit.cpp"

    #include "LuxEngine/Core/Memory/Ram/Ram.cpp"
    #include "LuxEngine/Core/Memory/Ram/Cell_t.cpp"

    #include "LuxEngine/System/System.cpp"

    #include "LuxEngine/Core/Devices.cpp"
    #include "LuxEngine/Core/Memory/VRam/VRam.cpp"
    #include "LuxEngine/Core/Memory/VRam/VCell_t.cpp"

    #include "LuxEngine/Core/Render/GCommands.cpp"
    #include "LuxEngine/Core/Render/Shaders/Shader.cpp"
    #include "LuxEngine/Core/Render/Window/Window.cpp" //FIXME
    #include "LuxEngine/Core/Render/Window/Swapchain.cpp"
    #include "LuxEngine/Core/Core.cpp"


    #include "LuxEngine/Threads/ThreadPool.cpp"
        //#include "LuxEngine/Math/Trigonometry/GoniometricFunctions.cpp"

    #include "LuxEngine/Types/LuxObject/Obj_b.cpp"
    #include "LuxEngine/Types/LuxObject/1D/Obj1_b.cpp"
    #include "LuxEngine/Types/LuxObject/2D/Obj2_b.cpp"
    #include "LuxEngine/Types/LuxObject/3D/Obj3_b.cpp"

    #include "LuxEngine/Types/LuxObject/2D/Line2.cpp"
    #include "LuxEngine/Types/LuxObject/2D/Border2.cpp"
    #include "LuxEngine/Types/LuxObject/2D/RenderSpace2.cpp"

    //TODO AUTOMATIZE
    #include "LuxEngine/shaders/Line2.cpp"
    #include "LuxEngine/shaders/Border2.cpp"
    #include "LuxEngine/shaders/FloatToIntBuffer.cpp"


    #include "LuxEngine/Core/Render/Buffers.cpp"
    #include "LuxEngine/Core/Render/Render.cpp"

    #include "LuxEngine/Core/Input/Input.cpp"
    #include "LuxEngine/Core/Input/InputState.cpp"
#endif