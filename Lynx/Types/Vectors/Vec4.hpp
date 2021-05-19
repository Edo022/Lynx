#pragma once
#define LNX_H_VEC4
#include "Lynx/macros.hpp"
#include "Lynx/Types/Vectors/Vec_b.hpp"



namespace lnxc{
	template<class t> struct vec4_t : vec_b {
		t x = 0;
		t y = 0;
		t z = 0;
		t w = 0;


		//Constructors
		alwaysInline constexpr vec4_t() noexcept {}
		alwaysInline constexpr vec4_t(const t& _x, const t& _y, const t& _z, const t& _w) noexcept { x = _x;     y = _y;     z = _z;     w = _w;     }
		alwaysInline constexpr vec4_t(const vec2_t<t>& _xy, const t& _z, const t& _w)        noexcept { x = _xy.x;  y = _xy.y;  z = z;      w =_w;      }
		alwaysInline constexpr vec4_t(const t& _x, const vec2_t<t>& _yz, const t& _w)        noexcept { x = _x;     y = _yz.x;  z = _yz.y;  w = _w;     }
		alwaysInline constexpr vec4_t(const t& _x, const t& _y, const vec2_t<t>& _zw)        noexcept { x = _x;     y = _y;     z = _zw.x;  w = _zw.y;  }
		alwaysInline constexpr vec4_t(const vec2_t<t>& _xy, const vec2_t<t>& _zw)               noexcept { x = _xy.x;  y = _xy.y;  z = _zw.x;  w = _zw.y;  }
		alwaysInline constexpr vec4_t(const vec3_t<t>& _xyz, const t& _w)                       noexcept { x = _xyz.x; y = _xyz.y; z = _xyz.z; w = _w;     }
		alwaysInline constexpr vec4_t(const t& _x, const vec3_t<t>& _yzw)                       noexcept { x = _x;     y = _yzw.y; z = _yzw.z; w = _yzw.w; }
		// explicit alwaysInline constexpr vec4_t(const vec4_t<t>& v) noexcept { *this = v; }
				alwaysInline constexpr vec4_t(const vec4_t<t>&    v) noexcept { *this = v; }
		// explicit alwaysInline constexpr vec4_t(const t& v        ) noexcept { *this = v; }
				alwaysInline constexpr vec4_t(const t&    v        ) noexcept { *this = v; }


		//Assignment operators, constructors and comparison operators
		template<class t_> inline constexpr auto& operator=(const vec4_t<t_>& v) noexcept requires(!std::is_same_v<t_, t>) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
		inline constexpr auto& operator=(const vec4_t<t>& v)    noexcept { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
		inline constexpr auto& operator=(const auto& n)         noexcept { x =      y =      z =   n; w =   n; return *this; }
		inline constexpr void operator==(const vec4_t<auto>& v) noexcept { return x == v.x && y == v.y && z == v.z && w == v.w ; }
		inline constexpr void operator==(const auto& n)         noexcept { return x == n   && y == n   && z == n   && w == n   ; }


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
}
#ifndef LNX_NO_GLOBAL_NAMESPACE
	using namespace lnxc;
#endif