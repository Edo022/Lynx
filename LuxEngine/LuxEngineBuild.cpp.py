

#The compiled file is only written right before its compilation to prevent C++ Intellisense from slowing down


f = open("./LuxEngine/LuxEngineBuild.cpp", "w");

f.write("\
    #include \"LuxEngine/Core/InitCore.cpp\"\n\
    \n\
    \n\
    \n\
    #include \"LuxEngine/Memory/Memory_t.cpp\"\n\
    #include \"LuxEngine/Memory/Ram/Memory.cpp\"\n\
    #include \"LuxEngine/Memory/Ram/Memory2.cpp\"\n\
    #include \"LuxEngine/Memory/Gpu/VMemory.cpp\"\n\
    \n\
    #include \"LuxEngine/System/System.cpp\"\n\
    #include \"LuxEngine/Threads/ThreadPool.cpp\"\n\
    #include \"LuxEngine/Math/Trigonometry/GoniometricFunctions.cpp\"\n\
    \n\
    #include \"LuxEngine/Types/LuxObject/LuxObject.cpp\"\n\
    #include \"LuxEngine/Types/LuxObject/2D/2DBorder.cpp\"\n\
    #include \"LuxEngine/Types/LuxObject/2D/2DLines.cpp\"\n\
    #include \"LuxEngine/Types/LuxObject/2D/2DRenderSpace.cpp\"\n\
    \n\
    #include \"LuxEngine/Core/Devices.cpp\"\n\
    #include \"LuxEngine/Core/Core.cpp\"\n\
    \n\
    \n\
    \n\
    #include \"LuxEngine/Core/Render/CBuffers.cpp\"\n\
    #include \"LuxEngine/Core/Render/Compute.cpp\"\n\
    #include \"LuxEngine/Core/Render/CShader.cpp\"\n\
    #include \"LuxEngine/Core/Render/GCommands.cpp\"\n\
    #include \"LuxEngine/Core/Render/GOutput.cpp\"\n\
    #include \"LuxEngine/Core/Render/GSwapchain.cpp\"\n\
    #include \"LuxEngine/Core/Render/Render.cpp\"\n\
    #include \"LuxEngine/Core/Render/Window.cpp\"\n\
    \n\
    #include \"LuxEngine/Core/Input/Input.cpp\"\n\
    #include \"LuxEngine/Core/Input/InputState.cpp\"\n\
    \n\
    \n\
    \n\
    #include \"LuxEngine/Core/StartCore.cpp\"\n\
    "
)

f.close();