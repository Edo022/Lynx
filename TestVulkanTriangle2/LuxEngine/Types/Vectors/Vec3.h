#pragma once

template<class t>
struct _vec3 {
	t x = 0;
	t y = 0;
	t z = 0;


	//Constructors																										
	inline _vec3() {}
	inline _vec3(const t& _x, const t& _y, const t& _z) { x = _x; y = _y; z = _z; }
	inline _vec3(const t& _x, const _vec2<t>& v) { x = _x; y = v.x; z = v.y; }
	inline _vec3(const _vec2<t>& v, const t& _z) { x = v.x; y = v.y; z = _z; }

	//Assignment operators and constructors																					
	inline _vec3(const _vec3<t>& v) { *this = v; }
	inline _vec3(const t& n) { *this = n; }
	inline void __vectorcall operator = (const _vec3<t>& v) { x = v.x; y = v.y; z = v.z; }
	inline void __vectorcall operator = (const t& n) { x = y = z = n; }

	//Add, subtract, multiply and divide operators																		
	inline _vec3<t> __vectorcall operator + (const _vec3<t>& v) const { return _vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline _vec3<t> __vectorcall operator * (const _vec3<t>& v) const { return _vec3<t>(x * v.x, y * v.y, z * v.z); }
	inline _vec3<t> __vectorcall operator - (const _vec3<t>& v) const { return _vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline _vec3<t> __vectorcall operator / (const _vec3<t>& v) const { return _vec3<t>(x / v.x, y / v.y, z / v.z); }
	inline void __vectorcall operator += (const _vec3<t>& v) { init(x + v.x, y + v.y, z + v.z); }
	inline void __vectorcall operator *= (const _vec3<t>& v) { init(x * v.x, y * v.y, z * v.z); }
	inline void __vectorcall operator -= (const _vec3<t>& v) { init(x - v.x, y - v.y, z - v.z); }
	inline void __vectorcall operator /= (const _vec3<t>& v) { init(x / v.x, y / v.y, z / v.z); }

private:
	inline void __vectorcall init(const t _x, const t _y, const t _z) { x = _x; y = _y; z = _z; }
};

