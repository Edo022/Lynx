#pragma once

template<class t> struct vec4_t {
	t x = 0;
	t y = 0;
	t z = 0;
	t w = 0;


	//Constructors																												
	inline vec4_t() {}
	inline vec4_t(const t& _x, const t& _y, const t& _z, const t& _w) { x = _x; y = _y; z = _z; w = _w; }
	inline vec4_t(const vec2_t<t>& xy, const vec2_t<t>& zw) { x = xy.x; y = xy.y; z = zw.z; w = zw.w; }
	inline vec4_t(const vec3_t<t>& v, const t& _w) { x = v.x; y = v.y; z = v.z; w = _w; }
	inline vec4_t(const t& _x, const vec3_t<t>& v) { x = _x; y = v.y; z = v.z; w = v.w; }
	inline vec4_t(const vec2_t<t>& v, const t& _z, const t& _w) { x = v.x; y = v.y; z = _z; w = _w; }
	inline vec4_t(const t& _x, const t& _y, const vec2_t<t>& v) { x = _x; y = _y; z = v.z; w = v.w; }

	//Assignment operators and constructors																							
	inline vec4_t(const vec4_t<t>& v) { *this = v; }
	inline vec4_t(const t& n) { *this = n; }
	inline void __vectorcall operator = (const vec4_t<t>& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline void __vectorcall operator = (const t& n) { x = y = z = w = n; }

	//Add, subtract, multiply and divide operators																				
	inline vec4_t<t> __vectorcall operator + (const vec4_t<t>& v) const { return vec4_t<t>(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline vec4_t<t> __vectorcall operator * (const vec4_t<t>& v) const { return vec4_t<t>(x * v.x, y * v.y, z * v.z, w * v.w); }
	inline vec4_t<t> __vectorcall operator - (const vec4_t<t>& v) const { return vec4_t<t>(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline vec4_t<t> __vectorcall operator / (const vec4_t<t>& v) const { return vec4_t<t>(x / v.x, y / v.y, z / v.z, w / v.w); }
	inline void __vectorcall operator += (const vec4_t<t>& v) { init(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline void __vectorcall operator *= (const vec4_t<t>& v) { init(x * v.x, y * v.y, z * v.z, w * v.w); }
	inline void __vectorcall operator -= (const vec4_t<t>& v) { init(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline void __vectorcall operator /= (const vec4_t<t>& v) { init(x / v.x, y / v.y, z / v.z, w / v.w); }

private:
	inline void __vectorcall init(const t _x, const t _y, const t _z, const t _w) { x = _x; y = _y; z = _z;  w = _w; }
};