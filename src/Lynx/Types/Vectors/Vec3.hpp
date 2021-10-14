#pragma once
#include "Lynx/Utils.hpp"
#include "Lynx/Types/Vectors/Vec_b.hpp"



namespace lnxc{
	//A tridimensional vector
	//Supports
	//   +, *, -, /, =, ==, *=, +=, -=, /=, %, %= operations with all types of vectors or values
	//   dist, dist3D, absv, signv, length functions with vectors of the same type
	//== operator does not perform any type cast
	template<class t> struct vec3_t : vec_b {
		t x = 0;
		t y = 0;
		t z = 0;


		//Constructors
		alwaysInline constexpr vec3_t() noexcept {}
		alwaysInline constexpr vec3_t(const t& _x, const t& _y, const t& _z) noexcept { x = _x;    y = _y;    z = _z;    }
		alwaysInline constexpr vec3_t(const t& _x, const vec2_t<t>& _yz)     noexcept { x = _x;    y = _yz.x; z = _yz.y; }
		alwaysInline constexpr vec3_t(const vec2_t<t>& _xy, const t& _z)     noexcept { x = _xy.x; y = _xy.y; z = z;     }
		alwaysInline constexpr vec3_t(const vec3_t<t>& v) noexcept { *this = v; }
		alwaysInline constexpr vec3_t(const t&         v) noexcept { *this = v; }


		//Assignment operators, constructors and comparison operators
		template<class t_> inline constexpr auto& operator=(const vec3_t<t_>& v) noexcept requires(!std::is_same_v<t_, t>) { x = v.x; y = v.y; z = v.z; return *this; }
		inline constexpr auto& operator=(const vec3_t<t>& v)    noexcept { x = v.x; y = v.y; z = v.z; return *this; }
		inline constexpr auto& operator=(const auto& n)         noexcept { x =      y =      z =   n; return *this; }
		inline constexpr void operator==(const vec3_t<auto>& v) noexcept { return x == v.x && y == v.y && z == v.z; }
		inline constexpr void operator==(const auto& n)         noexcept { return x == n   && y == n   && z == n;   }


		//Add, subtract, multiply and divide operators with vectors
		inline constexpr vec3_t<t> operator+(const vec3_t<auto>& v) const noexcept { return { x + v.x, y + v.y, z + v.z }; }
		inline constexpr vec3_t<t> operator*(const vec3_t<auto>& v) const noexcept { return { x * v.x, y * v.y, z * v.z }; }
		inline constexpr vec3_t<t> operator-(const vec3_t<auto>& v) const noexcept { return { x - v.x, y - v.y, z - v.z }; }
		inline constexpr vec3_t<t> operator/(const vec3_t<auto>& v) const noexcept { return { x / v.x, y / v.y, z / v.z }; }
		inline constexpr vec3_t<t> operator%(const vec3_t<auto>& v) const noexcept { return { x % v.x, y % v.y, z % v.z }; }
		inline constexpr void operator+=(const vec3_t<auto>& v) noexcept { x += v.x; y += v.y; z += v.z; }
		inline constexpr void operator*=(const vec3_t<auto>& v) noexcept { x *= v.x; y *= v.y; z *= v.z; }
		inline constexpr void operator-=(const vec3_t<auto>& v) noexcept { x -= v.x; y -= v.y; z -= v.z; }
		inline constexpr void operator/=(const vec3_t<auto>& v) noexcept { x /= v.x; y /= v.y; z /= v.z; }
		inline constexpr void operator%=(const vec3_t<auto>& v) noexcept { x %= v.x; y %= v.y; z %= v.z; }


		//Add, subtract, multiply and divide operators with values
		inline constexpr vec3_t<t> operator+(const auto& n) const noexcept { return { x + n, y + n, z + n }; }
		inline constexpr vec3_t<t> operator*(const auto& n) const noexcept { return { x * n, y * n, z * n }; }
		inline constexpr vec3_t<t> operator-(const auto& n) const noexcept { return { x - n, y - n, z - n }; }
		inline constexpr vec3_t<t> operator/(const auto& n) const noexcept { return { x / n, y / n, z / n }; }
		inline constexpr vec3_t<t> operator%(const auto& n) const noexcept { return { x % n, y % n, z % n }; }
		inline constexpr void operator+=(const auto& n) noexcept { x += n; y += n; z += n; }
		inline constexpr void operator*=(const auto& n) noexcept { x *= n; y *= n; z *= n; }
		inline constexpr void operator-=(const auto& n) noexcept { x -= n; y -= n; z -= n; }
		inline constexpr void operator/=(const auto& n) noexcept { x /= n; y /= n; z /= n; }
		inline constexpr void operator%=(const auto& n) noexcept { x %= n; y %= n; z %= n; }
	};
}
#ifndef LNX_NO_GLOBAL_NAMESPACE
	using namespace lnxc;
#endif