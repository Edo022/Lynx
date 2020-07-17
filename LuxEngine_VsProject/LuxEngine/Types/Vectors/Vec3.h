#pragma once

template<class t>
struct vec3_t {
	t x = 0;
	t y = 0;
	t z = 0;


	//Constructors
	inline vec3_t() {}
	inline vec3_t(const t& _x, const t& _y, const t& _z) { x = _x; y = _y; z = _z; }
	inline vec3_t(const t& _x, const vec2_t<t>& v) { x = _x; y = v.x; z = v.y; }
	inline vec3_t(const vec2_t<t>& v, const t& _z) { x = v.x; y = v.y; z = _z; }

	//Assignment operators and constructors
	inline vec3_t(const vec3_t<t>& v) { *this = v; }
	inline vec3_t(const t& n) { *this = n; }
	inline void __vectorcall operator = (const vec3_t<t>& v) { x = v.x; y = v.y; z = v.z; }
	inline void __vectorcall operator = (const t& n) { x = y = z = n; }

	//Add, subtract, multiply and divide operators
	inline vec3_t<t> __vectorcall operator + (const vec3_t<t>& v) const { return vec3_t<t>(x + v.x, y + v.y, z + v.z); }
	inline vec3_t<t> __vectorcall operator * (const vec3_t<t>& v) const { return vec3_t<t>(x * v.x, y * v.y, z * v.z); }
	inline vec3_t<t> __vectorcall operator - (const vec3_t<t>& v) const { return vec3_t<t>(x - v.x, y - v.y, z - v.z); }
	inline vec3_t<t> __vectorcall operator / (const vec3_t<t>& v) const { return vec3_t<t>(x / v.x, y / v.y, z / v.z); }
	inline void __vectorcall operator += (const vec3_t<t>& v) { init(x + v.x, y + v.y, z + v.z); }
	inline void __vectorcall operator *= (const vec3_t<t>& v) { init(x * v.x, y * v.y, z * v.z); }
	inline void __vectorcall operator -= (const vec3_t<t>& v) { init(x - v.x, y - v.y, z - v.z); }
	inline void __vectorcall operator /= (const vec3_t<t>& v) { init(x / v.x, y / v.y, z / v.z); }

private:
	inline void __vectorcall init(const t _x, const t _y, const t _z) { x = _x; y = _y; z = _z; }
};
