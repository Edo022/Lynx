#pragma once
#define LUX_H_CONTAINER_BASE
#include "LuxEngine/Types/Nothing.hpp"
#include "LuxEngine/Types/Nothing_sc_p.hpp"


namespace lux {
	template <class type, class iter> class ContainerBase {
	public:
		lux_sc_generate_debug_structure_body;

		virtual inline type*	begin( ) const = 0;		//Returns a pointer to the first element of the container
		virtual inline type*	end(   ) const = 0;		//Returns a pointer to the element after the last element of the container
		virtual inline iter		count( ) const = 0;		//Returns the number of elements in the container
		virtual inline uint64	size(  ) const = 0;		//Returns the size in bytes of the data contained in the container
		virtual inline bool		empty( ) const = 0;		//Returns true if the container has size 0, false otherwise
	};
}