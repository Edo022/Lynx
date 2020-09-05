#pragma once



namespace lux{
	//A struct that contains nothing and does nothing
	//Used for constructors that doesn't initialize the structure members or other very specific cases
	struct Nothing{ };


	//A function used as parameter in a constructor to not make it initialize the structure
	static constexpr inline const Nothing DontInitialize( ){ return Nothing{ }; }
}