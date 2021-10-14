#pragma once
#include "Lynx/Utils.hpp"
#include "Lynx/Types/Vectors/Vec_b.hpp"



namespace lnxc{
	//A bidimensional vector
	//Supports
	//   +, *, -, /, =, ==, *=, +=, -=, /= operations with vectors or values of every type
	//   dist, dist2D, absv, signv, length functions with vectors of the same type
	//   initialization and copy constructors with vectors or values of every type
	//== operator does not perform any type cast
	template<class t> struct vec2_t : vec_b {
		t x = 0;
		t y = 0;


		//Constructors
		alwaysInline constexpr vec2_t() noexcept {}
		alwaysInline constexpr vec2_t(const t& _x, const t& _y) noexcept { x = _x; y = _y; }
		alwaysInline constexpr vec2_t(const vec2_t<t>& v) noexcept { *this = v; }
		alwaysInline constexpr vec2_t(const t& v)         noexcept { *this = v; }


		//Assignment operators, constructors and comparison operators
		template<class t_> inline constexpr auto& operator=(const vec2_t<t_>& v) noexcept requires(!std::is_same_v<t_, t>) { x = v.x; y = v.y; return *this; }
		inline constexpr auto& operator=(const vec2_t<t>& v)    noexcept { x = v.x; y = v.y; return *this; }
		inline constexpr auto& operator=(const auto& n)         noexcept { x = n;   y = n;   return *this; }
		inline constexpr void operator==(const vec2_t<auto>& v) noexcept { return x == v.x && y == v.y; }
		inline constexpr void operator==(const auto& n)         noexcept { return x == n   && y == n;   }


		//Add, subtract, multiply and divide operators with vectors
		inline constexpr vec2_t<t> operator+(const vec2_t<auto>& v) const noexcept { return { x + v.x, y + v.y }; }
		inline constexpr vec2_t<t> operator*(const vec2_t<auto>& v) const noexcept { return { x * v.x, y * v.y }; }
		inline constexpr vec2_t<t> operator-(const vec2_t<auto>& v) const noexcept { return { x - v.x, y - v.y }; }
		inline constexpr vec2_t<t> operator/(const vec2_t<auto>& v) const noexcept { return { x / v.x, y / v.y }; }
		inline constexpr vec2_t<t> operator%(const vec2_t<auto>& v) const noexcept { return { x % v.x, y % v.y }; }
		inline constexpr void operator+=(const vec2_t<auto>& v) noexcept { x += v.x; y += v.y; }
		inline constexpr void operator*=(const vec2_t<auto>& v) noexcept { x *= v.x; y *= v.y; }
		inline constexpr void operator-=(const vec2_t<auto>& v) noexcept { x -= v.x; y -= v.y; }
		inline constexpr void operator/=(const vec2_t<auto>& v) noexcept { x /= v.x; y /= v.y; }
		inline constexpr void operator%=(const vec2_t<auto>& v) noexcept { x %= v.x; y %= v.y; }


		//Add, subtract, multiply and divide operators with values
		inline constexpr vec2_t<t> operator+(const auto& n) const noexcept { return { x + n, y + n }; }
		inline constexpr vec2_t<t> operator*(const auto& n) const noexcept { return { x * n, y * n }; }
		inline constexpr vec2_t<t> operator-(const auto& n) const noexcept { return { x - n, y - n }; }
		inline constexpr vec2_t<t> operator/(const auto& n) const noexcept { return { x / n, y / n }; }
		inline constexpr vec2_t<t> operator%(const auto& n) const noexcept { return { x % n, y % n }; }
		inline constexpr void operator+=(const auto& n) noexcept { x += n; y += n; }
		inline constexpr void operator*=(const auto& n) noexcept { x *= n; y *= n; }
		inline constexpr void operator-=(const auto& n) noexcept { x -= n; y -= n; }
		inline constexpr void operator/=(const auto& n) noexcept { x /= n; y /= n; }
		inline constexpr void operator%=(const auto& n) noexcept { x %= n; y %= n; }
	};
}
#ifndef LNX_NO_GLOBAL_NAMESPACE
	using namespace lnxc;
#endif