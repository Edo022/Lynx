
//This file was generated automatically. Changes can be overwritten without notice
#include <Line2D.hpp>



namespace lux::shd::Line2D{
	uint32 colorOutput_::bind = 0;
	colorOutput_::colorOutput_(){
		Shader_b::type = Storage;
	}

	uint32 windowSize_::bind = 1;
	windowSize_::windowSize_(){
		Shader_b::type = Storage;
	}

	uint32 zBuffer_::bind = 2;
	zBuffer_::zBuffer_(){
		Shader_b::type = Storage;
	}

	uint32 lineData_::bind = 3;
	lineData_::lineData_(){
		Shader_b::type = Uniform;
	}
}