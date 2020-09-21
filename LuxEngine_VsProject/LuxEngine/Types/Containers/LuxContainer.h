#pragma once
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"


namespace lux {
	template <class type, class iter> class ContainerBase {
	public:
		lux_sc_generate_debug_structure_body;

		virtual inline type* __vectorcall begin( ) const = 0;
		virtual inline type* __vectorcall end( ) const = 0;
		virtual inline iter __vectorcall size( ) const = 0;
		virtual inline uint64 __vectorcall bytes( ) const = 0;
		virtual inline bool __vectorcall empty( ) const = 0;
	};
}