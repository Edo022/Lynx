
#pragma once
#include "LuxEngine/Math/Constants.h"


template<class t> static inline t __vectorcall revToRad(t n) { return n * PI2; }		//Converts a value from revolutions to radiants
template<class t> static inline t __vectorcall radToRev(t n) { return n / PI2; }		//Converts a value from radiants to revolutions

template<class t> static inline t __vectorcall revToDeg(t n) { return n * 360; }		//Converts a value from revolutions to degrees
template<class t> static inline t __vectorcall degToRev(t n) { return n / 360; }		//Converts a value from degrees to revolutions

template<class t> static inline t __vectorcall radToDeg(t n) { return n * (180 / PI); }	//Converts a value from radiants to degrees
template<class t> static inline t __vectorcall degToRad(t n) { return n * (PI / 180); }	//Converts a value from degrees to radiants

