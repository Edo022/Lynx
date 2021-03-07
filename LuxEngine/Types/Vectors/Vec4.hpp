#pragma once
#include "LuxEngine/macros.hpp"




template<class t> struct vec4_t {
	t x = 0;
	t y = 0;
	t z = 0;
	t w = 0;


	//Constructors
	alwaysInline vec4_t() {}
	alwaysInline vec4_t(const auto& _x, const auto& _y, const auto& _z, const auto& _w) { x = _x;     y = _y;     z = _z;     w = _w;     }
	alwaysInline vec4_t(const vec2_t<auto>& _xy, const auto& _z, const auto& _w)        { x = _xy.x;  y = _xy.y;  z = z;      w =_w;      }
	alwaysInline vec4_t(const auto& _x, const vec2_t<auto>& _yz, const auto& _w)        { x = _x;     y = _yz.x;  z = _yz.y;  w = _w;     }
	alwaysInline vec4_t(const auto& _x, const auto& _y, const vec2_t<auto>& _zw)        { x = _x;     y = _y;     z = _zw.x;  w = _zw.y;  }
	alwaysInline vec4_t(const vec2_t<auto>& _xy, const vec2_t<auto>& _zw)               { x = _xy.x;  y = _xy.y;  z = _zw.x;  w = _zw.y;  }
	alwaysInline vec4_t(const vec3_t<auto>& _xyz, const auto& _w)                       { x = _xyz.x; y = _xyz.y; z = _xyz.z; w = _w;     }
	alwaysInline vec4_t(const auto& _x, const vec3_t<auto>& _yzw)                       { x = _x;     y = _yzw.y; z = _yzw.z; w = _yzw.w; }
	explicit alwaysInline vec4_t(const vec4_t<auto>& v) { *this = v; }
	         alwaysInline vec4_t(const vec4_t<t>&    v) { *this = v; }
	explicit alwaysInline vec4_t(const auto& v        ) { *this = v; }
	         alwaysInline vec4_t(const t&    v        ) { *this = v; }


	//Assignment operators, constructors and comparison operators
	inline void operator= (const vec4_t<auto>& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline void operator= (const auto& n        ) { x =      y =      z =   n; w =   n; }
	inline void operator==(const vec4_t<auto>& v) { return x == v.x && y == v.y && z == v.z && w == v.w ; }
	inline void operator==(const auto& n        ) { return x == n   && y == n   && z == n   && w == n   ; }


	//Add, subtract, multiply and divide operators with vectors
	inline vec4_t<t> operator+(const vec4_t<auto>& v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
	inline vec4_t<t> operator*(const vec4_t<auto>& v) const { return { x * v.x, y * v.y, z * v.z, w * v.w }; }
	inline vec4_t<t> operator-(const vec4_t<auto>& v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
	inline vec4_t<t> operator/(const vec4_t<auto>& v) const { return { x / v.x, y / v.y, z / v.z, w / v.w }; }
	inline vec4_t<t> operator%(const vec4_t<auto>& v) const { return { x % v.x, y % v.y, z % v.z, w % v.w }; }
	inline void operator+=(const vec4_t<auto>& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
	inline void operator*=(const vec4_t<auto>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	inline void operator-=(const vec4_t<auto>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	inline void operator/=(const vec4_t<auto>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }
	inline void operator%=(const vec4_t<auto>& v) { x %= v.x; y %= v.y; z %= v.z; w %= v.w; }


	//Add, subtract, multiply and divide operators with values
	inline vec4_t<t> operator+(const auto& n) const { return { x + n, y + n, z + n, w + n }; }
	inline vec4_t<t> operator*(const auto& n) const { return { x * n, y * n, z * n, w * n }; }
	inline vec4_t<t> operator-(const auto& n) const { return { x - n, y - n, z - n, w - n }; }
	inline vec4_t<t> operator/(const auto& n) const { return { x / n, y / n, z / n, w / n }; }
	inline vec4_t<t> operator%(const auto& n) const { return { x % n, y % n, z % n, w % n }; }
	inline void operator+=(const auto& n) { x += n; y += n; z += n; w += n; }
	inline void operator*=(const auto& n) { x *= n; y *= n; z *= n; w *= n; }
	inline void operator-=(const auto& n) { x -= n; y -= n; z -= n; w -= n; }
	inline void operator/=(const auto& n) { x /= n; y /= n; z /= n; w /= n; }
	inline void operator%=(const auto& n) { x %= n; y %= n; z %= n; w %= n; }
};