#pragma once
#include "LuxEngine/macros.h"

template<class t> struct vec3_t;
template<class t> struct vec4_t;


template<class t>
struct vec2_t {
	t x = 0;
	t y = 0;


	//Constructors
	inline vec2_t() {}
	inline vec2_t(const t& _x, const t& _y) { x = _x; y = _y; }

	//Assignment operators and constructors
	inline vec2_t(const vec2_t<t>& v) { *this = v; }
	inline vec2_t(const t& v) { *this = v; }
	template<class vt> inline vec2_t(const vec2_t<vt>& v) { *this = v;}
	template<class vt> inline vec2_t(const vt& v) { *this = v; }
	inline void __vectorcall operator = (const vec2_t<t>& v) { x = v.x; y = v.y; }
	inline void __vectorcall operator = (const t& n) { x = y = n; }
	template<class vt> inline void __vectorcall operator = (const vec2_t<vt>& v) { x = (t)(v.x); y = (t)(v.y); }
	template<class vt> inline void __vectorcall operator = (const vt& n) { x = y = (t)n; }

	//Add, subtract, multiply and divide operators
	template<class vt> inline vec2_t<t> __vectorcall operator + (const vec2_t<vt>& v) const { return vec2_t<t>{ scast<t>(x + v.x), scast<t>(y + v.y) }; }
	template<class vt> inline vec2_t<t> __vectorcall operator * (const vec2_t<vt>& v) const { return vec2_t<t>{ scast<t>(x * v.x), scast<t>(y * v.y) }; }
	template<class vt> inline vec2_t<t> __vectorcall operator - (const vec2_t<vt>& v) const { return vec2_t<t>{ scast<t>(x - v.x), scast<t>(y - v.y) }; }
	template<class vt> inline vec2_t<t> __vectorcall operator / (const vec2_t<vt>& v) const { return vec2_t<t>{ scast<t>(x / v.x), scast<t>(y / v.y) }; }
	template<class vt> inline void __vectorcall operator += (const vec2_t<t>& v) { init(x + v.x, y + v.y); }
	template<class vt> inline void __vectorcall operator *= (const vec2_t<t>& v) { init(x * v.x, y * v.y); }
	template<class vt> inline void __vectorcall operator -= (const vec2_t<t>& v) { init(x - v.x, y - v.y); }
	template<class vt> inline void __vectorcall operator /= (const vec2_t<t>& v) { init(x / v.x, y / v.y); }



private:
	inline void __vectorcall init(const t& _x, const t& _y) { x = _x; y = _y; }
};