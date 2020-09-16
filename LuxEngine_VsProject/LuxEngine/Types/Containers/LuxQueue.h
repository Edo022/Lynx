#pragma once
#include "LuxDynArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO automatic back to front and viceversa in queues

//A queue with dynamic size
namespace lux {
	template<class type, class iter = uint32> struct Queue {
		lux_sc_generate_debug_structure_body;
		lux::DynArray<type, iter> _front, _back;

		lux_sc_generate_nothing_constructor(Queue) _front(getConstNothing( )), _back(getConstNothing( )) { }
		inline Queue( ) : _front(0xFF), _back(0xFF) { }


		//Adds an element at the back of the queue
		inline void pushBack(const type& vElement)	{ lux_sc_F; _back.add(vElement); }
		//Adds an element at the front of the queue
		inline void pushFront(const type& vElement) { lux_sc_F; _front.add(vElement); }
		//Removes an element from the back of the queue
		inline void popBack(const type& vElement)	{ lux_sc_F; _back.resize(_back.size( ) - 1); }
		//Removes an element from the front of the queue
		inline void popFront( )						{ lux_sc_F; _front.resize(_front.size( ) - 1); }


		inline type& operator [](const iter vIndex) { lux_sc_F; return (vIndex < _back.size( )) ? _back[_back.size( ) - 1 - vIndex] : _front[vIndex - (_back.size( ) - 1)]; }
		inline type& front( )	{ lux_sc_F; return *_front.end( ); }
		inline type& back( )	{ lux_sc_F; return *_back.end( ); }
		inline iter size( )		{ lux_sc_F; return _front.size( ) + _back.size( ); }
		inline bool empty( )	{ lux_sc_F; return size( ) == 0; }
	};
}
