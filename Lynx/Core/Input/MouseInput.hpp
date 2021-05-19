#pragma once
#define LNX_H_MOUSE_INPUT



namespace lnx{
	enum MouseButton {
		eN1     = 0,		//Same as MouseButton::right
		eN2     = 1,		//Same as MouseButton::left
		eN3     = 2,		//Same as MouseButton::middle
		eN4     = 3,
		eN5     = 4,
		eN6     = 5,
		eN7     = 6,
		eN8     = 7,
		eRight  = eN1,	//Right click
		eLeft   = eN2,	//Left click
		eMiddle = eN3		//Middle click
	};
}