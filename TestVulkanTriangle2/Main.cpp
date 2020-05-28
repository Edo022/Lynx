
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "RenderEngine/Render.h"
#include "Types/Types.h"
#include <thread>
#include "macros.h"

//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$


/*
	LD 0
		queue 0
			command pool 0
				command buffer 0
					command 0
					command 1
					command 2
					...
				...
			...
		...
	LD 1
		...
	...
*/


MdrObject object("renderTexture");


Render render;
void run() { 
	//TODO move to render
	mdrObjectLoadObj(&object, "./Contents/Models/modelloBrutto.obj");
	mdrSpawnObject(&render, &object);

	render.run(false, 45);
}


int main() {
	std::thread t(run);
	t.detach();

	render.running = true;
	while (render.running) {
		Sleep(1);
	}

	return 0;
}