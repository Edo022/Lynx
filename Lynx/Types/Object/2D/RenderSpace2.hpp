#pragma once
#define LNX_H_RENDER_SPACE_2D
#include "Lynx/Types/Object/RenderSpace_b.hpp"




namespace lnx::obj {
	enum class AlignmentType : int16{
		Horizontal,
		Vertical,
		FixedHorizontal,
		FixedVertical,
		FixedGrid,
		Free
	};

	enum class FlowType : int16{
		LeftToRight,
		RightToLeft,
		TopToBottom,
		BottomToTop
	};


	//TODO add object boundary debug draw


	/**
	 * @brief A 2D space that allows objects to be rendered.
	 *		This render space can only be a child of a window or another RenderSpace2
	 */
	struct RenderSpace2 : public RenderSpace_b {
		bool allowOutOfViewRender = false;			//Allow out of limit object regions to be rendered

		AlignmentType alignment;	//Type of children alignmen
		uint32 xNum;				//Number of horizontally aligned children
		uint32 yNum;				//Number of vertically aligned children
		FlowType xFlow;				//Children horizontal direction
		FlowType yFlow;				//Children vertical direction


		RenderSpace2(AlignmentType vAlignment, uint32 vXNum = 2, uint32 vYNum = 2, FlowType vXFlow = FlowType::LeftToRight, FlowType vYFlow = FlowType::TopToBottom)
			: alignment(vAlignment), xNum(vXNum), yNum(vYNum), xFlow(vXFlow), yFlow(vYFlow) {
		}


		bool addChild(Obj2_b* pObject);
		bool setChildLimits(const uint32 vChildIndex) const final;



		#define gtollx(n) ((n) / render.parentWindow->swp.createInfo.imageExtent.width) //FIXME DONT DEPEND ON A WINDOW
		#define gtolly(n) ((n) / render.parentWindow->swp.createInfo.imageExtent.height)
		void onMove(f32v2 pos) override {
			setMaxLim(f32v2{ ((float32)gtollx(pos.x)), ((float32)gtolly(pos.y)) });
			qHierarchy();
		}
	};
}
