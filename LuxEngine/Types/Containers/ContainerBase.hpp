#pragma once
#define LUX_H_CONTAINER_BASE
#include "LuxEngine/Types/Nothing.hpp"
#include "LuxEngine/Types/Nothing_sc_p.hpp"


namespace lux {
	template <class type, class iter> class ContainerBase {
	public:
		lux_sc_generate_debug_structure_body;

		virtual inline type*	begin( ) const = 0;
		virtual inline type*	end(   ) const = 0;
		virtual inline iter		count( ) const = 0;
		virtual inline uint64	size(  ) const = 0;
		virtual inline bool		empty( ) const = 0;
	};
}