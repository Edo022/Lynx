#pragma once
#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Types/Integers/Integers.hpp"


namespace lux::obj {
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


	//TODO add click events
	//TODO add object boundary debug draw


	//A 2D space that allows objects to be rendered
	//The position of the object is relative to the count and position of the render space
	//    It goes from 0, 0 (beginning of the render space) to +1, +1 (end of the render space)
	//    Out of view objects will be cropped if the renderSpaces does not allowOutOfViewRender
	//    Use setChildLimits() to get the region of screen assigned to a child of the render space
	//This render space can only be a child of other 2D render spaces
	//Render spaces with no parent will be rendered directly in the window
	struct RenderSpace2D : public Base2D {
		bool allowOutOfViewRender = false;			//Allow out of limit object regions to be rendered

		AlignmentType alignment;	//Type of children alignmen
		uint32 xNum;				//Number of horizontally aligned children
		uint32 yNum;				//Number of vertically aligned children
		FlowType xFlow;				//Children horizontal direction
		FlowType yFlow;				//Children vertical direction


		RenderSpace2D(AlignmentType alignment, uint32 xNum = 2, uint32 yNum = 2, FlowType xFlow = FlowType::LeftToRight, FlowType yFlow = FlowType::TopToBottom)
			: alignment(alignment), xNum(xNum), yNum(yNum), xFlow(xFlow), yFlow(yFlow) {
			luxInitObject(2, RENDER_SPACE);
		}


		bool addChild(Base2D* pObject);
		bool setChildLimits(const uint32 vChildIndex) const final override;

		//! 0 BY DEFAULT
		// inline int32 getCellSize( ) const final override { return 0; }
	};
}
//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space
