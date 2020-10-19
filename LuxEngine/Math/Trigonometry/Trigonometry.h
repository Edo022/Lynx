
#pragma once
#include "LuxEngine/Math/Constants.h"


template<class t> static inline t revToRad(t n) { return n * PI2; }		//Converts a value from revolutions to radiants
template<class t> static inline t radToRev(t n) { return n / PI2; }		//Converts a value from radiants to revolutions

template<class t> static inline t revToDeg(t n) { return n * 360; }		//Converts a value from revolutions to degrees
template<class t> static inline t degToRev(t n) { return n / 360; }		//Converts a value from degrees to revolutions

template<class t> static inline t radToDeg(t n) { return n * (180 / PI); }	//Converts a value from radiants to degrees
template<class t> static inline t degToRad(t n) { return n * (PI / 180); }	//Converts a value from degrees to radiants

