#pragma once

template<class t> struct _vec3;
template<class t> struct _vec4;


template<class t>
struct _vec2 {
	t x = 0;
	t y = 0;


	//Constructors																									
	inline _vec2() {}
	inline _vec2(const t& _x, const t& _y) { x = _x; y = _y; }

	//Assignment operators and constructors																				
	inline _vec2(const _vec2<t>& v) { *this = v; }
	inline _vec2(const t& v) { *this = v; }
	inline void __vectorcall operator = (const _vec2<t>& v) { x = v.x; y = v.y; }
	inline void __vectorcall operator = (const t& n) { x = y = n; }

	//Add, subtract, multiply and divide operators																	
	inline _vec2<t> __vectorcall operator + (const _vec2<t>& v) const { return _vec2<t>(x + v.x, y + v.y); }
	inline _vec2<t> __vectorcall operator * (const _vec2<t>& v) const { return _vec2<t>(x * v.x, y * v.y); }
	inline _vec2<t> __vectorcall operator - (const _vec2<t>& v) const { return _vec2<t>(x - v.x, y - v.y); }
	inline _vec2<t> __vectorcall operator / (const _vec2<t>& v) const { return _vec2<t>(x / v.x, y / v.y); }
	inline void __vectorcall operator += (const _vec2<t>& v) { init(x + v.x, y + v.y); }
	inline void __vectorcall operator *= (const _vec2<t>& v) { init(x * v.x, y * v.y); }
	inline void __vectorcall operator -= (const _vec2<t>& v) { init(x - v.x, y - v.y); }
	inline void __vectorcall operator /= (const _vec2<t>& v) { init(x / v.x, y / v.y); }




	////Swizzling
	//#define __lp_vec2_swizzling(a, b)		inline _vec2<t> __vectorcall a##b()			{ return _vec2<t>(a, b); }
	//#define __lp_vec3_swizzling(a, b, c)	inline _vec3<t> __vectorcall a##b##c()		{ return _vec3<t>(a, b, c); }
	//#define __lp_vec4_swizzling(a, b, c, d)	inline _vec4<t> __vectorcall a##b##c##d()	{ return _vec4<t>(a, b, c, d); }

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
