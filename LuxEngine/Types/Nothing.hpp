#pragma once



namespace lux{
	//A struct that contains nothing and does nothing
	struct Nothing{ };



	//Used by constructors of some Lux structures to skip the default object initialization by initializing its members with their own value
	static constexpr inline const Nothing getConstNothing( ){ return Nothing{ }; }
}