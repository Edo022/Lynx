#pragma once
#include <math.h>



template<class t>
struct _vec2 {
	t x = 0;
	t y = 0;


	_vec2() {}
	_vec2(t _x, t _y) { x = _x; y = _y; }

	_vec2<t> operator +	(_vec2<t>& v) { return _vec2<t>(x + v.x, y + v.y); }
	_vec2<t> operator *	(_vec2<t>& v) { return _vec2<t>(x * v.x, y * v.y); }
	_vec2<t> operator -	(_vec2<t>& v) { return _vec2<t>(x - v.x, y - v.y); }
	_vec2<t> operator /	(_vec2<t>& v) { return _vec2<t>(x / v.x, y / v.y); }
	void operator +=	(_vec2<t>& v) { init(x + v.x, y + v.y); }
	void operator *=	(_vec2<t>& v) { init(x * v.x, y * v.y); }
	void operator -=	(_vec2<t>& v) { init(x - v.x, y - v.y); }
	void operator /=	(_vec2<t>& v) { init(x / v.x, y / v.y); }

	void operator =		(t n) { x = y = n; }

private:
	void init(t _x, t _y) { x = _x; y = _y; }
};