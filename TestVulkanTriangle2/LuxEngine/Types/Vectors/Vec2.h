#pragma once


template<class t>
struct _vec2 {
	t x = 0;
	t y = 0;


	//Constructors																									
	inline _vec2() {}
	inline _vec2(const t& _x, const t& _y) { x = _x; y = _y; }

	//Assignment operators and constructors																				
	inline _vec2(const _vec2<t>& v) { *this = v; }
	inline _vec2(const t& v) { *this = v; }
	inline void __vectorcall operator = (const _vec2<t>& v) { x = v.x; y = v.y; }
	inline void __vectorcall operator = (const t& n) { x = y = n; }

	//Add, subtract, multiply and divide operators																	
	inline _vec2<t> __vectorcall operator + (const _vec2<t>& v) const { return _vec2<t>(x + v.x, y + v.y); }
	inline _vec2<t> __vectorcall operator * (const _vec2<t>& v) const { return _vec2<t>(x * v.x, y * v.y); }
	inline _vec2<t> __vectorcall operator - (const _vec2<t>& v) const { return _vec2<t>(x - v.x, y - v.y); }
	inline _vec2<t> __vectorcall operator / (const _vec2<t>& v) const { return _vec2<t>(x / v.x, y / v.y); }
	inline void __vectorcall operator += (const _vec2<t>& v) { init(x + v.x, y + v.y); }
	inline void __vectorcall operator *= (const _vec2<t>& v) { init(x * v.x, y * v.y); }
	inline void __vectorcall operator -= (const _vec2<t>& v) { init(x - v.x, y - v.y); }
	inline void __vectorcall operator /= (const _vec2<t>& v) { init(x / v.x, y / v.y); }

private:
	inline void __vectorcall init(const t& _x, const t& _y) { x = _x; y = _y; }
};
