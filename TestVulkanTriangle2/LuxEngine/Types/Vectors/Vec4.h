#pragma once

template<class t>
struct _vec4 {
	t x = 0;
	t y = 0;
	t z = 0;
	t w = 0;


	//Constructors																												
	inline _vec4() {}
	inline _vec4(const t& _x, const t& _y, const t& _z, const t& _w) { x = _x; y = _y; z = _z; w = _w; }
	inline _vec4(const _vec2<t>& xy, const _vec2<t>& zw) { x = xy.x; y = xy.y; z = zw.z; w = zw.w; }
	inline _vec4(const _vec3<t>& v, const t& _w) { x = v.x; y = v.y; z = v.z; w = _w; }
	inline _vec4(const t& _x, const _vec3<t>& v) { x = _x; y = v.y; z = v.z; w = v.w; }
	inline _vec4(const _vec2<t>& v, const t& _z, const t& _w) { x = v.x; y = v.y; z = _z; w = _w; }
	inline _vec4(const t& _x, const t& _y, const _vec2<t>& v) { x = _x; y = _y; z = v.z; w = v.w; }

	//Assignment operators and constructors																							
	inline _vec4(const _vec4<t>& v) { *this = v; }
	inline _vec4(const t& n) { *this = n; }
	inline void __vectorcall operator = (const _vec4<t>& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline void __vectorcall operator = (const t& n) { x = y = z = w = n; }

	//Add, subtract, multiply and divide operators																				
	inline _vec4<t> __vectorcall operator + (const _vec4<t>& v) const { return _vec4<t>(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline _vec4<t> __vectorcall operator * (const _vec4<t>& v) const { return _vec4<t>(x * v.x, y * v.y, z * v.z, w * v.w); }
	inline _vec4<t> __vectorcall operator - (const _vec4<t>& v) const { return _vec4<t>(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline _vec4<t> __vectorcall operator / (const _vec4<t>& v) const { return _vec4<t>(x / v.x, y / v.y, z / v.z, w / v.w); }
	inline void __vectorcall operator += (const _vec4<t>& v) { init(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline void __vectorcall operator *= (const _vec4<t>& v) { init(x * v.x, y * v.y, z * v.z, w * v.w); }
	inline void __vectorcall operator -= (const _vec4<t>& v) { init(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline void __vectorcall operator /= (const _vec4<t>& v) { init(x / v.x, y / v.y, z / v.z, w / v.w); }

private:
	inline void __vectorcall init(const t _x, const t _y, const t _z, const t _w) { x = _x; y = _y; z = _z;  w = _w; }
};