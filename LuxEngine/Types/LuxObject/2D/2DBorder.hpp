#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"
#include "LuxEngine/Contents/shaders/Border2D.hpp"



namespace lux::obj{
	struct Border2D : public Obj2_b{
		// f32v2* fp = nullptr;
		// f32v2* sp = nullptr;
		shd::Border2D data;


		Border2D() {
			// init();
			//FIXME ...
			cellSize = 40;
			luxInitObject(2, BORDER);							// count   | range         | chunk
			// fp = (f32v2*)(render.data + 0);						//    8    |    0  - 7     |    0 +
			// sp = (f32v2*)(render.data + 8);						//    8    |    8  - 15    |    0
			// *(uint32*)(render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
			// //4x trailing padding								//    4    |    20 - 23

			// *fp = { .0f, .0f };
			// *sp = { .0f, .0f };
			data.objData_.ffp() = { .0f, .0f };
			data.objData_.fsp() = { .0f, .0f };
			data.objData_.ID() = (uint32)common.ID;
		}
		Border2D(const f32v2& vFp, const f32v2& vSp) {
			// init();
			//FIXME ...
			cellSize = 40;
			luxInitObject(2, BORDER);							// count   | range         | chunk
			// fp = (f32v2*)(render.data + 0);						//    8    |    0  - 7     |    0 +
			// sp = (f32v2*)(render.data + 8);						//    8    |    8  - 15    |    0
			// *(uint32*)(render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
			// //4x trailing padding								//    4    |    20 - 23

			// *fp = vFp;
			// *sp = vSp;
			data.objData_.ffp() = vFp;
			data.objData_.fsp() = vSp;
			data.objData_.ID() = (uint32)common.ID;
		}

		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void init(Window& pWindow) override;
	};
}