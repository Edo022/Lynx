#pragma once

template<class t> struct vec3_t;
template<class t> struct vec4_t;



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
	alwaysInline vec2_t(const auto& _x, const auto& _y) { x = (t)_x; y = (t)_y; }




	//Assignment operators, constructors and comparison operators
	/**/			   alwaysInline vec2_t(const vec2_t<t>& v ) { *this = v; }
	/**/			   alwaysInline vec2_t(const t& v         ) { *this = v; }
	template<class vt> alwaysInline vec2_t(const vt& v        ) { *this = v; }
	template<class vt> explicit inline vec2_t(const vec2_t<vt>& v) { *this = v; }
	/**/			   inline void operator=(const vec2_t<t>& v ) { x = v.x; y = v.y;           }
	/**/			   inline void operator=(const t& n         ) { x =      y = n;             }
	template<class vt> inline void operator=(const vec2_t<vt>& v) { x = (t)(v.x); y = (t)(v.y); }
	template<class vt> inline void operator=(const vt& n        ) { x =           y = (t)n;     }
	template<class vt> inline bool operator!=(const vec2_t<vt>& v) const { return x != v.x || y != v.y;    }
	template<class vt> inline bool operator!=(const vt& n        ) const { return x != n   || y != n;      }
	template<class vt> inline bool operator==(const vec2_t<vt>& v) const { return !(x != v.x || y != v.y); }
	template<class vt> inline bool operator==(const vt& n        ) const { return !(x != n   || y != n);   }




	//Add, subtract, multiply and divide operators with vectors
	template<class vt> inline vec2_t<t> operator+(const vec2_t<vt>& v) const { return { x + v.x, y + v.y }; }
	template<class vt> inline vec2_t<t> operator*(const vec2_t<vt>& v) const { return { x * v.x, y * v.y }; }
	template<class vt> inline vec2_t<t> operator-(const vec2_t<vt>& v) const { return { x - v.x, y - v.y }; }
	template<class vt> inline vec2_t<t> operator/(const vec2_t<vt>& v) const { return { x / v.x, y / v.y }; }

	template<class vt> inline void operator+=(const vec2_t<vt>& v) { x += v.x; y += v.y; }
	template<class vt> inline void operator*=(const vec2_t<vt>& v) { x *= v.x; y *= v.y; }
	template<class vt> inline void operator-=(const vec2_t<vt>& v) { x -= v.x; y -= v.y; }
	template<class vt> inline void operator/=(const vec2_t<vt>& v) { x /= v.x; y /= v.y; }

	template<class vt> inline bool  operator<(const vec2_t<vt>& v) { return ((x < v.x) && (y < v.y));   }
	template<class vt> inline bool  operator>(const vec2_t<vt>& v) { return ((x > v.x) && (y > v.y));   }
	template<class vt> inline bool operator<=(const vec2_t<vt>& v) { return ((x <= v.x) && (y <= v.y)); }
	template<class vt> inline bool operator>=(const vec2_t<vt>& v) { return ((x >= v.x) && (y >= v.y)); }




	//Add, subtract, multiply and divide operators with values
	template<class vt> inline vec2_t<t> operator+(const vt& n) const { return { x + n, y + n }; }
	template<class vt> inline vec2_t<t> operator*(const vt& n) const { return { x * n, y * n }; }
	template<class vt> inline vec2_t<t> operator-(const vt& n) const { return { x - n, y - n }; }
	template<class vt> inline vec2_t<t> operator/(const vt& n) const { return { x / n, y / n }; }

	template<class vt> inline void operator+=(const vt& n) { x += n; y += n; }
	template<class vt> inline void operator*=(const vt& n) { x *= n; y *= n; }
	template<class vt> inline void operator-=(const vt& n) { x -= n; y -= n; }
	template<class vt> inline void operator/=(const vt& n) { x /= n; y /= n; }

	template<class vt> inline bool operator<(const vt& v ) { return ((x < v) && (y < v));   }
	template<class vt> inline bool operator>(const vt& v ) { return ((x > v) && (y > v));   }
	template<class vt> inline bool operator<=(const vt& v) { return ((x <= v) && (y <= v)); }
	template<class vt> inline bool operator>=(const vt& v) { return ((x >= v) && (y >= v)); }




	//Returns the length of the vector
	inline auto len() { return sqrt(x * x + y * y); }


private:
	inline void init(const t& _x, const t& _y) { x = _x; y = _y; }
};