#pragma once
#include "LuxEngine/macros.hpp"




//A tridimensional vector
//Supports
//   +, *, -, /, =, ==, *=, +=, -=, /=, %, %= operations with all types of vectors or values
//   dist, dist3D, absv, signv, length functions with vectors of the same type
//== operator does not perform any type cast

template<class t> struct vec3_t {
	t x = 0;
	t y = 0;
	t z = 0;


	//Constructors
	alwaysInline vec3_t() {}
	alwaysInline vec3_t(const auto& _x, const auto& _y, const auto& _z) { x = _x;    y = _y;    z = _z;    }
	alwaysInline vec3_t(const auto& _x, const vec2_t<auto>& _yz)        { x = _x;    y = _yz.x; z = _yz.y; }
	alwaysInline vec3_t(const vec2_t<auto>& _xy,        const auto& _z) { x = _xy.x; y = _xy.y; z = z;     }
	explicit alwaysInline vec3_t(const vec3_t<auto>& v) { *this = v; }
	         alwaysInline vec3_t(const vec3_t<t>&    v) { *this = v; }
	explicit alwaysInline vec3_t(const auto& v        ) { *this = v; }
	         alwaysInline vec3_t(const t&    v        ) { *this = v; }


	//Assignment operators, constructors and comparison operators
	inline void operator= (const vec3_t<auto>& v) { x = v.x; y = v.y; z = v.z; }
	inline void operator= (const auto& n        ) { x =      y =      z =   n; }
	inline void operator==(const vec3_t<auto>& v) { return x == v.x && y == v.y && z == v.z; }
	inline void operator==(const auto& n        ) { return x == n   && y == n   && z == n;   }


	//Add, subtract, multiply and divide operators with vectors
	inline vec3_t<t> operator+(const vec3_t<auto>& v) const { return { x + v.x, y + v.y, z + v.z }; }
	inline vec3_t<t> operator*(const vec3_t<auto>& v) const { return { x * v.x, y * v.y, z * v.z }; }
	inline vec3_t<t> operator-(const vec3_t<auto>& v) const { return { x - v.x, y - v.y, z - v.z }; }
	inline vec3_t<t> operator/(const vec3_t<auto>& v) const { return { x / v.x, y / v.y, z / v.z }; }
	inline vec3_t<t> operator%(const vec3_t<auto>& v) const { return { x % v.x, y % v.y, z % v.z }; }
	inline void operator+=(const vec3_t<auto>& v) { x += v.x; y += v.y; z += v.z; }
	inline void operator*=(const vec3_t<auto>& v) { x *= v.x; y *= v.y; z *= v.z; }
	inline void operator-=(const vec3_t<auto>& v) { x -= v.x; y -= v.y; z -= v.z; }
	inline void operator/=(const vec3_t<auto>& v) { x /= v.x; y /= v.y; z /= v.z; }
	inline void operator%=(const vec3_t<auto>& v) { x %= v.x; y %= v.y; z %= v.z; }


	//Add, subtract, multiply and divide operators with values
	inline vec3_t<t> operator+(const auto& n) const { return { x + n, y + n, z + n }; }
	inline vec3_t<t> operator*(const auto& n) const { return { x * n, y * n, z * n }; }
	inline vec3_t<t> operator-(const auto& n) const { return { x - n, y - n, z - n }; }
	inline vec3_t<t> operator/(const auto& n) const { return { x / n, y / n, z / n }; }
	inline vec3_t<t> operator%(const auto& n) const { return { x % n, y % n, z % n }; }
	inline void operator+=(const auto& n) { x += n; y += n; z += n; }
	inline void operator*=(const auto& n) { x *= n; y *= n; z *= n; }
	inline void operator-=(const auto& n) { x -= n; y -= n; z -= n; }
	inline void operator/=(const auto& n) { x /= n; y /= n; z /= n; }
	inline void operator%=(const auto& n) { x %= n; y %= n; z %= n; }
};
