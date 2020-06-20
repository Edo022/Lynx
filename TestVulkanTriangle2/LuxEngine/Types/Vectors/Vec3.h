#pragma once

#define __lp_vec3_struct_def(name, ref) 																					\
template<class t>																											\
struct name {																												\
	t x = 0;																												\
	t y = 0;																												\
	t z = 0;																												\
																															\
																															\
	/*Constructors																											*/\
	inline __vectorcall name() {}																							\
	inline __vectorcall name(const t##ref _x, const t##ref _y, const t##ref _z) { x = _x; y = _y; z = _z; }					\
	inline __vectorcall name(const t##ref _x, const _vec2<t>##ref v) { x = _x; y = v.x; z = v.y; }							\
	inline __vectorcall name(const _vec2<t>##ref v, const t##ref _z) { x = v.x; y = v.y; z = _z; }							\
																															\
	/*Equal operators and constructors																						*/\
	inline __vectorcall name(const name<t>& v) { *this = v; }																\
	inline __vectorcall name(const t##ref n) { *this = n; }																	\
	inline void __vectorcall operator = (const name<t>##ref v) { x = v.x; y = v.y; z = v.z; }								\
	inline void __vectorcall operator = (const t##ref n) { x = y = z = n; }													\
																															\
	/*Add, subtract, multiply and divide operators																			*/\
	inline name<t> __vectorcall operator + (const name<t>##ref v) const { return name<t>(x + v.x, y + v.y, z + v.z); }		\
	inline name<t> __vectorcall operator * (const name<t>##ref v) const { return name<t>(x * v.x, y * v.y, z * v.z); }		\
	inline name<t> __vectorcall operator - (const name<t>##ref v) const { return name<t>(x - v.x, y - v.y, z - v.z); }		\
	inline name<t> __vectorcall operator / (const name<t>##ref v) const { return name<t>(x / v.x, y / v.y, z / v.z); }		\
	inline void __vectorcall operator += (const name<t>##ref v) { init(x + v.x, y + v.y, z + v.z); }						\
	inline void __vectorcall operator *= (const name<t>##ref v) { init(x * v.x, y * v.y, z * v.z); }						\
	inline void __vectorcall operator -= (const name<t>##ref v) { init(x - v.x, y - v.y, z - v.z); }						\
	inline void __vectorcall operator /= (const name<t>##ref v) { init(x / v.x, y / v.y, z / v.z); }						\
																															\
private:																													\
	inline void __vectorcall init(const t _x, const t _y, const t _z) { x = _x; y = _y; z = _z; }							\
};																															


__lp_vec3_struct_def(_vec3, )		//Default vec3 with inputs passed by-value. Better performance with types smaller than 8 bytes
__lp_vec3_struct_def(_vec3ref, &)	//Additional vec3 with inputs passed by-reference. Better performance with types larger than 8 bytes
#undef __lp_vec3_struct_def