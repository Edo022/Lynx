#pragma once


namespace lux {
	template <class type, class iter> class ContainerBase {
	public:
		virtual inline type* __vectorcall begin( ) const = 0;
		virtual inline type* __vectorcall end( ) const = 0;
		virtual inline iter __vectorcall size( ) const = 0;
		virtual inline uint64 __vectorcall bytes( ) const = 0;
		virtual inline bool __vectorcall empty( ) const = 0;
	};
}