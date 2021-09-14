CPP=g++
CPPFLAGS=-std=c++20 -I. -Ofast -g3
# SRC=Lynx/System/SystemInfo.cpp Lynx/Tests/StructureInit.cpp Lynx/Core/Memory/Ram/Ram.cpp Lynx/Core/Memory/Ram/Cell_t.cpp Lynx/System/System.cpp Lynx/Core/Devices.cpp Lynx/Core/Memory/VRam/VRam.cpp Lynx/Core/Memory/VRam/VCell_t.cpp Lynx/Core/Render/GCommands.cpp Lynx/Core/Render/Shaders/Shader.cpp Lynx/Core/Render/Window/Window.cpp Lynx/Core/Render/Window/Swapchain.cpp Lynx/Core/Core.cpp Lynx/Threads/ThreadPool.cpp Lynx/Types/Object/Obj_b.cpp Lynx/Types/Object/2D/Line2.cpp Lynx/Types/Object/2D/Border2.cpp Lynx/Types/Object/2D/RenderSpace2.cpp Lynx/Types/Object/2D/RenderSpace3.cpp Lynx/Types/Object/3D/Volume.cpp Lynx/shaders/Line2.cpp Lynx/shaders/Border2.cpp Lynx/shaders/FloatToIntBuffer.cpp Lynx/shaders/3DTest.cpp Lynx/Core/Render/Buffers.cpp Lynx/Core/Render/Render.cpp Lynx/Core/Input/Input.cpp Lynx/Core/Input/InputState.cpp
BINS=$(addprefix ./bin/,$(shell basename -a $(SRC:.cpp=.o)))


all: $(BINS)
	ar -rcs ./bin/lynxengine.a $^ 

$(BINS): $(SRC)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm ./bin/*