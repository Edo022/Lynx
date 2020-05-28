#pragma once


#include "Vec2.h"
#include "Vec3.h"
#include "Types/Integers/Integers.h"

#include <cinttypes>
#include <math.h>


typedef _vec2<char>			charv2,		cvec2;
typedef _vec2<int32>		int32v2,	vec2;
typedef _vec2<int64>		int64v2,	lvec2;
typedef _vec2<int128>		int128v2,	llvec2;
typedef _vec2<float>		floatv2,	fvec2;
typedef _vec2<double>		doublev2,	dvec2;

typedef _vec2<char>			charv3,		cvec3;
typedef _vec3<int32>		int32v3,	vec3;
typedef _vec3<int64>		int64v3,	lvec3;
typedef _vec3<int128>		int128v3,	llvec3;
typedef _vec3<float>		floatv3,	fvec3;
typedef _vec3<double>		doublev3,	dvec3;






template<class t>
static inline int32 dist(_vec2<t> a, _vec2<t> b) { return (sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))); }
template<class t>
static inline int64 ldist(_vec2<t> a, _vec2<t> b) { return (sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))); }
template<class t>
static inline double ddist(_vec2<t> a, _vec2<t> b) { return (sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))); }


template<class t>
static inline int32 dist(_vec3<t> a, _vec3<t> b) { return (sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2))); }
template<class t>
static inline int64 ldist(_vec3<t> a, _vec3<t> b) { return (sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2))); }
template<class t>
static inline double ddist(_vec3<t> a, _vec3<t> b) { return (sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2))); }
