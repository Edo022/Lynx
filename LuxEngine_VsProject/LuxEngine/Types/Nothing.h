#pragma once



namespace lux{
	//A struct that contains nothing and does nothing
	struct Nothing{ };


	//Used by constructors of some Lux structures to skip the default object initialization by initializing its members with their own value
	//Use it only if you have already initialized the structure. Initializing an uninitialized structure with its value is ub
	//e.g.  lux::Array<int> UwU(lux::DontInitialize( ));
	static constexpr inline const Nothing DontInitialize( ){ return Nothing{ }; }
}