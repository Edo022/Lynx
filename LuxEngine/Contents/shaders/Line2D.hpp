#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/VPointer.hpp>



namespace lux::shd::Line2D{
	struct colorOutput_{
		static bufferType type;
		static uint32     bind;
		f32v4 colorOutput[];
	};

	struct windowSize_{
		static bufferType type;
		static uint32     bind;
		u32 width; u32 height;
	};

	struct zBuffer_{
		static bufferType type;
		static uint32     bind;
		u32 zBuffer[];
	};

	struct lineData_{
		static bufferType type;
		static uint32     bind;
		f32v2 fp0;      //0
		f32v2 fp1;      //8
		f32v4 col0;     //16
		f32v4 col1;     //32
		f32 wd0;        //48
		f32 wd1;        //52
		u32 ID;         //56
	};
}