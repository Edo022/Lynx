#pragma once


#define __lp_vec2_struct_def(name, ref) 																					\
template<class t>																											\
struct name {																												\
	t x = 0;																												\
	t y = 0;																												\
																															\
																															\
	/*Constructors																											*/\
	inline name() {}																										\
	inline name(const t##ref _x, const t##ref _y) { x = _x; y = _y; }														\
																															\
	/*Equal operators and constructors																						*/\
	inline name(const name<t>& v) { *this = v; }																			\
	inline name(const t##ref v) { *this = v; }																				\
	inline void __vectorcall operator = (const name<t>##ref v) { x = v.x; y = v.y; }										\
	inline void __vectorcall operator = (const t##ref n) { x = y = n; }														\
																															\
	/*Add, subtract, multiply and divide operators																			*/\
	inline name<t> __vectorcall operator + (const name<t>##ref v) const { return name<t>(x + v.x, y + v.y); }				\
	inline name<t> __vectorcall operator * (const name<t>##ref v) const { return name<t>(x * v.x, y * v.y); }				\
	inline name<t> __vectorcall operator - (const name<t>##ref v) const { return name<t>(x - v.x, y - v.y); }				\
	inline name<t> __vectorcall operator / (const name<t>##ref v) const { return name<t>(x / v.x, y / v.y); }				\
	inline void __vectorcall operator += (const name<t>##ref v) { init(x + v.x, y + v.y); }									\
	inline void __vectorcall operator *= (const name<t>##ref v) { init(x * v.x, y * v.y); }									\
	inline void __vectorcall operator -= (const name<t>##ref v) { init(x - v.x, y - v.y); }									\
	inline void __vectorcall operator /= (const name<t>##ref v) { init(x / v.x, y / v.y); }									\
																															\
private:																													\
	inline void __vectorcall init(const t##ref _x, const t##ref _y) { x = _x; y = _y; }										\
};


__lp_vec2_struct_def(_vec2, )		//Default vec2 with inputs passed by-value. Better performance with types smaller than 8 bytes
__lp_vec2_struct_def(_vec2ref, &)	//Additional vec2 with inputs passed by-reference. Better performance with types larger than 8 bytes
#undef __lp_vec2_struct_def