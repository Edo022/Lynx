#pragma once

#define __lp_vec4_struct_def(name, ref) 																							\
template<class t>																													\
struct name {																														\
	t x = 0;																														\
	t y = 0;																														\
	t z = 0;																														\
	t w = 0;																														\
																																	\
																																	\
	/*Constructors																													*/\
	inline name() {}																												\
	inline name(const t##ref _x, const t##ref _y, const t##ref _z, const t##ref _w) { x = _x; y = _y; z = _z; w = _w; }				\
	inline name(const _vec2<t>##ref xy, const _vec2<t>##ref zw) { x = xy.x; y = xy.y; z = zw.z; w = zw.w; }							\
	inline name(const _vec3<t>##ref v, const t##ref _w) { x = v.x; y = v.y; z = v.z; w = _w; }										\
	inline name(const t##ref _x, const _vec3<t>##ref v) { x = _x; y = v.y; z = v.z; w = v.w; }										\
	inline name(const _vec2<t>##ref v, const t##ref _z, const t##ref _w) { x = v.x; y = v.y; z = _z; w = _w; }						\
	inline name(const t##ref _x, const t##ref _y, const _vec2<t>##ref v) { x = _x; y = _y; z = v.z; w = v.w; }						\
																																	\
	/*Equal operators and constructors																								*/\
	inline name(const name<t>& v) { *this = v; }																					\
	inline name(const t##ref n) { *this = n; }																						\
	inline void __vectorcall operator = (const name<t>##ref v) { x = v.x; y = v.y; z = v.z; w = v.w; }								\
	inline void __vectorcall operator = (const t##ref n) { x = y = z = w = n; }														\
																																	\
	/*Add, subtract, multiply and divide operators																					*/\
	inline name<t> __vectorcall operator + (const name<t>##ref v) const { return name<t>(x + v.x, y + v.y, z + v.z, w + v.w); }		\
	inline name<t> __vectorcall operator * (const name<t>##ref v) const { return name<t>(x * v.x, y * v.y, z * v.z, w * v.w); }		\
	inline name<t> __vectorcall operator - (const name<t>##ref v) const { return name<t>(x - v.x, y - v.y, z - v.z, w - v.w); }		\
	inline name<t> __vectorcall operator / (const name<t>##ref v) const { return name<t>(x / v.x, y / v.y, z / v.z, w / v.w); }		\
	inline void __vectorcall operator += (const name<t>##ref v) { init(x + v.x, y + v.y, z + v.z, w + v.w); }						\
	inline void __vectorcall operator *= (const name<t>##ref v) { init(x * v.x, y * v.y, z * v.z, w * v.w); }						\
	inline void __vectorcall operator -= (const name<t>##ref v) { init(x - v.x, y - v.y, z - v.z, w - v.w); }						\
	inline void __vectorcall operator /= (const name<t>##ref v) { init(x / v.x, y / v.y, z / v.z, w / v.w); }						\
																																	\
private:																															\
	inline void __vectorcall init(const t _x, const t _y, const t _z, const t _w) { x = _x; y = _y; z = _z;  w = _w; }				\
};																															

#pragma warning( disable: 4003 )	//No enough arguments for macro
__lp_vec4_struct_def(_vec4, )		//Default vec4 with inputs passed by-value. Better performance with types smaller than 8 bytes
__lp_vec4_struct_def(_vec4ref, &)	//Additional vec4 with inputs passed by-reference. Better performance with types larger than 8 bytes
#pragma warning( default: 4003 )	//No enough arguments for macro
#undef __lp_vec4_struct_def