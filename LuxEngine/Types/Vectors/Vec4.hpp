#pragma once
#define LUX_H_VEC4
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Types/Vectors/Vec_b.hpp"



template<class t> struct vec4_t : vec_b {
	t x = 0;
	t y = 0;
	t z = 0;
	t w = 0;


	//Constructors
	alwaysInline constexpr vec4_t() noexcept {}
	alwaysInline constexpr vec4_t(const auto& _x, const auto& _y, const auto& _z, const auto& _w) noexcept { x = _x;     y = _y;     z = _z;     w = _w;     }
	alwaysInline constexpr vec4_t(const vec2_t<auto>& _xy, const auto& _z, const auto& _w)        noexcept { x = _xy.x;  y = _xy.y;  z = z;      w =_w;      }
	alwaysInline constexpr vec4_t(const auto& _x, const vec2_t<auto>& _yz, const auto& _w)        noexcept { x = _x;     y = _yz.x;  z = _yz.y;  w = _w;     }
	alwaysInline constexpr vec4_t(const auto& _x, const auto& _y, const vec2_t<auto>& _zw)        noexcept { x = _x;     y = _y;     z = _zw.x;  w = _zw.y;  }
	alwaysInline constexpr vec4_t(const vec2_t<auto>& _xy, const vec2_t<auto>& _zw)               noexcept { x = _xy.x;  y = _xy.y;  z = _zw.x;  w = _zw.y;  }
	alwaysInline constexpr vec4_t(const vec3_t<auto>& _xyz, const auto& _w)                       noexcept { x = _xyz.x; y = _xyz.y; z = _xyz.z; w = _w;     }
	alwaysInline constexpr vec4_t(const auto& _x, const vec3_t<auto>& _yzw)                       noexcept { x = _x;     y = _yzw.y; z = _yzw.z; w = _yzw.w; }
	explicit alwaysInline constexpr vec4_t(const vec4_t<auto>& v) noexcept { *this = v; }
	         alwaysInline constexpr vec4_t(const vec4_t<t>&    v) noexcept { *this = v; }
	explicit alwaysInline constexpr vec4_t(const auto& v        ) noexcept { *this = v; }
	         alwaysInline constexpr vec4_t(const t&    v        ) noexcept { *this = v; }


	//Assignment operators, constructors and comparison operators
	inline constexpr void operator= (const vec4_t<auto>& v) noexcept { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline constexpr void operator= (const auto& n        ) noexcept { x =      y =      z =   n; w =   n; }
	inline constexpr void operator==(const vec4_t<auto>& v) noexcept { return x == v.x && y == v.y && z == v.z && w == v.w ; }
	inline constexpr void operator==(const auto& n        ) noexcept { return x == n   && y == n   && z == n   && w == n   ; }


	//Add, subtract, multiply and divide operators with vectors
	inline constexpr vec4_t<t> operator+(const vec4_t<auto>& v) const noexcept { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
	inline constexpr vec4_t<t> operator*(const vec4_t<auto>& v) const noexcept { return { x * v.x, y * v.y, z * v.z, w * v.w }; }
	inline constexpr vec4_t<t> operator-(const vec4_t<auto>& v) const noexcept { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
	inline constexpr vec4_t<t> operator/(const vec4_t<auto>& v) const noexcept { return { x / v.x, y / v.y, z / v.z, w / v.w }; }
	inline constexpr vec4_t<t> operator%(const vec4_t<auto>& v) const noexcept { return { x % v.x, y % v.y, z % v.z, w % v.w }; }
	inline constexpr void operator+=(const vec4_t<auto>& v) noexcept { x += v.x; y += v.y; z += v.z; w += v.w; }
	inline constexpr void operator*=(const vec4_t<auto>& v) noexcept { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	inline constexpr void operator-=(const vec4_t<auto>& v) noexcept { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	inline constexpr void operator/=(const vec4_t<auto>& v) noexcept { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }
	inline constexpr void operator%=(const vec4_t<auto>& v) noexcept { x %= v.x; y %= v.y; z %= v.z; w %= v.w; }


	//Add, subtract, multiply and divide operators with values
	inline constexpr vec4_t<t> operator+(const auto& n) const noexcept { return { x + n, y + n, z + n, w + n }; }
	inline constexpr vec4_t<t> operator*(const auto& n) const noexcept { return { x * n, y * n, z * n, w * n }; }
	inline constexpr vec4_t<t> operator-(const auto& n) const noexcept { return { x - n, y - n, z - n, w - n }; }
	inline constexpr vec4_t<t> operator/(const auto& n) const noexcept { return { x / n, y / n, z / n, w / n }; }
	inline constexpr vec4_t<t> operator%(const auto& n) const noexcept { return { x % n, y % n, z % n, w % n }; }
	inline constexpr void operator+=(const auto& n) noexcept { x += n; y += n; z += n; w += n; }
	inline constexpr void operator*=(const auto& n) noexcept { x *= n; y *= n; z *= n; w *= n; }
	inline constexpr void operator-=(const auto& n) noexcept { x -= n; y -= n; z -= n; w -= n; }
	inline constexpr void operator/=(const auto& n) noexcept { x /= n; y /= n; z /= n; w /= n; }
	inline constexpr void operator%=(const auto& n) noexcept { x %= n; y %= n; z %= n; w %= n; }
};