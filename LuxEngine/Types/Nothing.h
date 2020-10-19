#pragma once



namespace lux{
	//A struct that contains nothing and does nothing
	struct Nothing{ };



	//Used by constructors of some Lux structures to skip the default object initialization by initializing its members with their own value
	static constexpr inline const Nothing getConstNothing( ){ return Nothing{ }; }


	//Optimized version of NoInitVar for Lux structures with a lux::Nothing constructor
	//e.g.  lux::Array<int> NoInitLux(bar);
	//Variables declared with this macro MUST be initialized before their default initialization
	#define NoInitLux(var) var(getConstNothing( ))


	//Used to declare a variable without initializing it
	//e.g.  int NoInitVar(UwU);
	//Variables declared with this macro MUST be initialized before their default initialization
	#define NoInitVar(var) var{ var }
}