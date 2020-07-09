#pragma once

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
	inline void __vectorcall operator = (const vec2_t<t>& v) { x = v.x; y = v.y; }
	inline void __vectorcall operator = (const t& n) { x = y = n; }

	//Add, subtract, multiply and divide operators																	
	inline vec2_t<t> __vectorcall operator + (const vec2_t<t>& v) const { return vec2_t<t>(x + v.x, y + v.y); }
	inline vec2_t<t> __vectorcall operator * (const vec2_t<t>& v) const { return vec2_t<t>(x * v.x, y * v.y); }
	inline vec2_t<t> __vectorcall operator - (const vec2_t<t>& v) const { return vec2_t<t>(x - v.x, y - v.y); }
	inline vec2_t<t> __vectorcall operator / (const vec2_t<t>& v) const { return vec2_t<t>(x / v.x, y / v.y); }
	inline void __vectorcall operator += (const vec2_t<t>& v) { init(x + v.x, y + v.y); }
	inline void __vectorcall operator *= (const vec2_t<t>& v) { init(x * v.x, y * v.y); }
	inline void __vectorcall operator -= (const vec2_t<t>& v) { init(x - v.x, y - v.y); }
	inline void __vectorcall operator /= (const vec2_t<t>& v) { init(x / v.x, y / v.y); }




	////Swizzling
	//#define __lp_vec2_swizzling(a, b)		inline vec2_t<t> __vectorcall a##b()			{ return vec2_t<t>(a, b); }
	//#define __lp_vec3_swizzling(a, b, c)	inline vec3_t<t> __vectorcall a##b##c()		{ return vec3_t<t>(a, b, c); }
	//#define __lp_vec4_swizzling(a, b, c, d)	inline vec4_t<t> __vectorcall a##b##c##d()	{ return vec4_t<t>(a, b, c, d); }

	//__lp_vec2_swizzling(y, x);
	//__lp_vec2_swizzling(x, x);
	//__lp_vec2_swizzling(y, y);




	//__lp_vec3_swizzling(x, x, x);
	//__lp_vec3_swizzling(y, y, y);

	//__lp_vec3_swizzling(x, y, y);
	//__lp_vec3_swizzling(y, x, y);
	//__lp_vec3_swizzling(y, y, x);

	//__lp_vec3_swizzling(y, x, x);
	//__lp_vec3_swizzling(x, y, x);
	//__lp_vec3_swizzling(x, x, y);




	//__lp_vec4_swizzling(x, x, x, x);
	//__lp_vec4_swizzling(y, y, y, y);

	//__lp_vec4_swizzling(y, x, x, x);
	//__lp_vec4_swizzling(x, y, x, x);
	//__lp_vec4_swizzling(x, x, y, x);
	//__lp_vec4_swizzling(x, x, x, y);

	//__lp_vec4_swizzling(x, y, y, y);
	//__lp_vec4_swizzling(y, x, y, y);
	//__lp_vec4_swizzling(y, y, x, y);
	//__lp_vec4_swizzling(y, y, y, x);

	//__lp_vec4_swizzling(x, x, y, y);
	//__lp_vec4_swizzling(y, x, x, y);
	//__lp_vec4_swizzling(y, y, x, x);
	//__lp_vec4_swizzling(x, y, y, x);




	//#undef __lp_vec2_swizzling
	//#undef __lp_vec3_swizzling
	//#undef __lp_vec4_swizzling

private:
	inline void __vectorcall init(const t& _x, const t& _y) { x = _x; y = _y; }
};