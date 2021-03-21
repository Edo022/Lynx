#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/VPointer.hpp>



namespace lux::shd::Line2D{
	struct colorOutput_{
		static bufferType type;
		static uint32     bind;
		alignas(4 * 4) f32v4 colorOutput[];
	};

	struct windowSize_{
		static bufferType type;
		static uint32     bind;
		alignas(1 * 4) u32 width; alignas(1 * 4) u32 height;
	};

	struct zBuffer_{
		static bufferType type;
		static uint32     bind;
		alignas(1 * 4) u32 zBuffer[];
	};

	struct lineData_{
		static bufferType type;
		static uint32     bind;
		alignas(4 * 4) f32v2 fp0;      //0
		alignas(4 * 4) f32v2 fp1;      //8
		alignas(4 * 4) f32v4 col0;     //16
		alignas(4 * 4) f32v4 col1;     //32
		alignas(1 * 4) f32 wd0;        //48
		alignas(1 * 4) f32 wd1;        //52
		alignas(1 * 4) u32 ID;         //56
	};
}