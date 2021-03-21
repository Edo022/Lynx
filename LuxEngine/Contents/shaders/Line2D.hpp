#pragma once
#include "LuxEngine/Types/Vectors/Vectors.hpp"



namespace lux::shd::Line2D{
	//Type: buffer, Binding: 0
	struct colorOutput_{
		f32v4 colorOutput[];
	};


	//Type: buffer, Binding: 1
	struct windowSize_{
		u32 width; ui32 height;
	};


	//Type: buffer, Binding: 2
	struct zBuffer_{
		u32 zBuffer[];
	};


	//Type: uniform, Binding: 3
	struct lineData_{
		//16 a;
		f32v2 fp0;      //0
		f32v2 fp1;      //8
		//16
		f32v4 col0 ;    //16
		//16
		f32v4 col1;     //32
		//12
		f32 wd0;        //48
		f32 wd1;        //52
		u32 ID;         //56
	};

}