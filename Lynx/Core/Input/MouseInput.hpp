#pragma once
#define LNX_H_MOUSE_INPUT



namespace lnx{
	enum MouseButton {
		n1     = 0,		//Same as MouseButton::right
		n2     = 1,		//Same as MouseButton::left
		n3     = 2,		//Same as MouseButton::middle
		n4     = 3,
		n5     = 4,
		n6     = 5,
		n7     = 6,
		n8     = 7,
		right  = n1,	//Right click
		left   = n2,	//Left click
		middle = n3		//Middle click
	};
}