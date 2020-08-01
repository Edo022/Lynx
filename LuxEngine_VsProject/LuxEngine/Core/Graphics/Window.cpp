
#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Graphics/Window.h"




namespace lux::core::g{
	GLFWwindow*		window;
	int32			width = 1920 * 2, height = 1080;
	FenceDE			windowResizeFence;
	LuxCell			gpuCellWindowSize;
	LuxCell			gpuCellWindowOutput;
	LuxCell			gpuCellWindowOutput_i;
}