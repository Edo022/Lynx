#pragma once


template<class t>
struct _vec3 {
	t x = 0;
	t y = 0;
	t z = 0;


	_vec3() {}
	_vec3(t _x, t _y, t _z) { x = _x; y = _y; z = _z; }
	_vec3(t _x, _vec2<t> v) { x = _x; y = v.x; z = v.y; }
	_vec3(_vec2<t> v, t _z) { x = v.x; y = v.y; z = _z; }

	_vec3 operator +	(_vec3& v) { return _vec3(x + v.x, y + v.y, z + v.z); }
	_vec3 operator *	(_vec3& v) { return _vec3(x * v.x, y * v.y, z * v.z); }
	_vec3 operator -	(_vec3& v) { return _vec3(x - v.x, y - v.y, z - v.z); }
	_vec3 operator /	(_vec3& v) { return _vec3(x / v.x, y / v.y, z / v.z); }
	void operator +=	(_vec3& v) { init(x + v.x, y + v.y, z + v.z); }
	void operator *=	(_vec3& v) { init(x * v.x, y * v.y, z * v.z); }
	void operator -=	(_vec3& v) { init(x - v.x, y - v.y, z - v.z); }
	void operator /=	(_vec3& v) { init(x / v.x, y / v.y, z / v.z); }

	void operator =		(t n) { x = y = z = n; }

private:
	void init(t _x, t _y, t _z) { x = _x; y = _y; z = _z; }
};