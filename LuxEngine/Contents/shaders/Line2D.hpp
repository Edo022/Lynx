
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd::Line2D{

	struct colorOutput_ : public Shader_b {
		 f32v4 colorOutput/*0*/
	};

	struct windowSize_ : public Shader_b {
		 u32 width/*0*/ u32 height/*4*/
	};

	struct zBuffer_ : public Shader_b {
		 u32 zBuffer/*0*/
	};

	struct lineData_ : public Shader_b {
		 f32v2 _vec2/*0*///0
		 f32v2 fp1/*8*///8
		//TEST
		//16
		/*TEST*///32
		 f32 wd0/*16*///48
		/*
		TEST
		    */ f32 floatname/*20*///52
		/*TEST  */
	};

	struct colorOutput_ : public Shader_b {
		 f32v4 colorOutput/*0*/
	};

	struct windowSize_ : public Shader_b {
		 u32 width/*0*/ u32 height/*4*/
	};

	struct zBuffer_ : public Shader_b {
		 u32 zBuffer/*0*/
	};

	struct lineData_ : public Shader_b {
		//  vec2 fp0;       //0
		//  vec2 fp1;       //8
		//  vec4 col0;      //16
		//  vec4 col1;      //32
		//  float wd0;      //48
		//  float wd1;      //52
		//  uint ID;        //56

	};}