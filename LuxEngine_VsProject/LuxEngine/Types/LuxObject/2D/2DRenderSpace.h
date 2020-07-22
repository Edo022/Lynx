#pragma once

#include "LuxEngine/Types/LuxObject/LuxObject.h"



namespace lux::obj {
	enum class AlignmentType : int8{
		Horizontal,
		Vertical,
		FixedHorizontal,
		FixedVertical,
		FixedGrid,
		Free
	};

	enum class FlowType : int8{
		LeftToRight,
		RightToLeft,
		TopToBottom,
		BottomToTop
	};


	//TODO add click events
	//TODO add object boundary debug draw


	//A 2D space that allows objects to be rendered
	//The position of the object is relative to the size and position of the render space
	//    It goes from 0, 0 (beginning of the render space) to +1, +1 (end of the render space)
	//    Out of view objects will be cropped if the renderSpaces does not allowOutOfViewRender
	//    Use setChildLimits() to get the region of screen assigned to a child of the render space
	//This render space can only be a child of other 2D render spaces
	//Render spaces with no parent will be rendered directly in the window
	struct RenderSpace2D : public Base2D {
		static const ObjectType objectType = LUX_OBJECT_TYPE_RENDER_SPACE_2D;
		bool allowOutOfViewRender = false;			//Allow out of limit object regions to be rendered

		AlignmentType alignment;	//Type of children alignmen
		uint32 xNum;				//Number of horizontally aligned children
		uint32 yNum;				//Number of vertically aligned children
		FlowType xFlow;				//Children horizontal direction
		FlowType yFlow;				//Children vertical direction


		RenderSpace2D(AlignmentType alignment, uint32 xNum = 2, uint32 yNum = 2, FlowType xFlow = FlowType::LeftToRight, FlowType yFlow = FlowType::TopToBottom)
			: alignment(alignment), xNum(xNum), yNum(yNum), xFlow(xFlow), yFlow(yFlow) {
		}



		//Adds an object to the render space children
		//Automatically updates the  parent and child index of the object
		//Returns the child index
		bool addChild(Base2D* pObject){
			pObject->parent = this;
			setChildLimits(pObject->childIndex = children.add(pObject));
			pObject->update( );
			return pObject->childIndex;
		}

		//Updates the render limit of the child with at specific index
		//It depends on the render space properties and children alignment
		//A negative index is equal to the n-th last element (using index -5 with a render space with 8 children is the same as using index 2)
		//Returns false if the index is invalid
		bool setChildLimits(const uint32 vChildIndex) const final override {
			if(vChildIndex >= children.size( )) return false;
			switch(alignment){
				case AlignmentType::FixedHorizontal:
				{
					auto xElmLen = abs(minLim.x - maxLim.x) / xNum;
					children[vChildIndex]->minLim = { minLim.x + (xElmLen * vChildIndex), minLim.y };
					children[vChildIndex]->maxLim = { minLim.x + (xElmLen * vChildIndex) + xElmLen, maxLim.y };
					break;
				}
				case AlignmentType::FixedVertical:
				{
					auto yElmLen = abs(minLim.y - maxLim.y) / xNum;
					children[vChildIndex]->minLim = { minLim.x, minLim.y + (yElmLen * vChildIndex) };
					children[vChildIndex]->maxLim = { maxLim.x, minLim.y + (yElmLen * vChildIndex) + yElmLen };
					break;
				}
				//case AlignmentType::Vertical:
				//case AlignmentType::Horizontal:
				case AlignmentType::Free:
					children[vChildIndex]->minLim = minLim;
					children[vChildIndex]->maxLim = maxLim;
					break;
				default:
					exit(-123);
			}
			return true;
		}


		void update( ) final override { }
		inline int32 getCellSize( ) const final override { return 0; }
	};
}
//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space