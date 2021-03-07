#pragma once
#include "LuxEngine/macros.hpp"




//A bidimensional vector
//Supports
//   +, *, -, /, =, ==, *=, +=, -=, /= operations with vectors or values of every type
//   dist, dist2D, absv, signv, length functions with vectors of the same type
//   initialization and copy constructors with vectors or values of every type
//== operator does not perform any type cast
template<class t> struct vec2_t {
	t x = 0;
	t y = 0;


	//Constructors
	alwaysInline vec2_t() {}
	alwaysInline vec2_t(const auto& _x, const auto& _y) { x = _x; y = _y; }
	explicit alwaysInline vec2_t(const vec2_t<auto>& v) { *this = v; }
	         alwaysInline vec2_t(const vec2_t<t>&    v) { *this = v; }
	explicit alwaysInline vec2_t(const auto& v        ) { *this = v; }
	         alwaysInline vec2_t(const t&    v        ) { *this = v; }


	//Assignment operators, constructors and comparison operators
	inline void operator= (const vec2_t<auto>& v) { x = v.x; y = v.y;}
	inline void operator= (const auto& n        ) { x = n;   y = n;  }
	inline void operator==(const vec2_t<auto>& v) { return x == v.x && y == v.y; }
	inline void operator==(const auto& n        ) { return x == n   && y == n;   }


	//Add, subtract, multiply and divide operators with vectors
	inline vec2_t<t> operator+(const vec2_t<auto>& v) const { return { x + v.x, y + v.y }; }
	inline vec2_t<t> operator*(const vec2_t<auto>& v) const { return { x * v.x, y * v.y }; }
	inline vec2_t<t> operator-(const vec2_t<auto>& v) const { return { x - v.x, y - v.y }; }
	inline vec2_t<t> operator/(const vec2_t<auto>& v) const { return { x / v.x, y / v.y }; }
	inline vec2_t<t> operator%(const vec2_t<auto>& v) const { return { x % v.x, y % v.y }; }
	inline void operator+=(const vec2_t<auto>& v) { x += v.x; y += v.y; }
	inline void operator*=(const vec2_t<auto>& v) { x *= v.x; y *= v.y; }
	inline void operator-=(const vec2_t<auto>& v) { x -= v.x; y -= v.y; }
	inline void operator/=(const vec2_t<auto>& v) { x /= v.x; y /= v.y; }
	inline void operator%=(const vec2_t<auto>& v) { x %= v.x; y %= v.y; }


	//Add, subtract, multiply and divide operators with values
	inline vec2_t<t> operator+(const auto& n) const { return { x + n, y + n }; }
	inline vec2_t<t> operator*(const auto& n) const { return { x * n, y * n }; }
	inline vec2_t<t> operator-(const auto& n) const { return { x - n, y - n }; }
	inline vec2_t<t> operator/(const auto& n) const { return { x / n, y / n }; }
	inline vec2_t<t> operator%(const auto& n) const { return { x % n, y % n }; }
	inline void operator+=(const auto& n) { x += n; y += n; }
	inline void operator*=(const auto& n) { x *= n; y *= n; }
	inline void operator-=(const auto& n) { x -= n; y -= n; }
	inline void operator/=(const auto& n) { x /= n; y /= n; }
	inline void operator%=(const auto& n) { x %= n; y %= n; }
};