#pragma once
#include "Lynx/Math/Constants.hpp"
#include "Lynx/macros.hpp"




//TODO probably useless
template<class t> static constexpr alwaysInline t revToRad(const t n) { return n * TAU; }		//Converts a value from revolutions to radiants
template<class t> static constexpr alwaysInline t radToRev(const t n) { return n / TAU; }		//Converts a value from radiants to revolutions

template<class t> static constexpr alwaysInline t revToDeg(const t n) { return n * 360; }		//Converts a value from revolutions to degrees
template<class t> static constexpr alwaysInline t degToRev(const t n) { return n / 360; }		//Converts a value from degrees to revolutions

template<class t> static constexpr alwaysInline t radToDeg(const t n) { return n * (180 / PI); }	//Converts a value from radiants to degrees
template<class t> static constexpr alwaysInline t degToRad(const t n) { return n * (PI / 180); }	//Converts a value from degrees to radiants

