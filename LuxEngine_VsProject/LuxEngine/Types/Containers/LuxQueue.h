#pragma once
#include "LuxDynArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO automatic back to front and viceversa in queues
//TODO FIX QUEUES

//A queue with dynamic size
namespace lux {
	template<class type, class iter = uint32> struct Queue {
		lux_sc_generate_debug_structure_body;
		lux::DynArray<type, iter> _front, _back;

		lux_sc_generate_nothing_constructor(Queue) _front(getConstNothing( )), _back(getConstNothing( )) { }
		inline Queue( ) : _front( ), _back( ) { }


		//Adds an element at the back of the queue
		inline void pushBack(const type& vElement)	{ checkInit; _back.add(vElement); }
		//Adds an element at the front of the queue
		inline void pushFront(const type& vElement) { checkInit; _front.add(vElement); }
		//Removes an element from the back of the queue
		inline void popBack(const type& vElement)	{ checkInit; _back.resize(_back.size( ) - 1); }
		//Removes an element from the front of the queue
		inline void popFront( )						{ checkInit; _front.resize(_front.size( ) - 1); }


		inline type& operator [](const iter vIndex) { checkInit; return (vIndex < _back.size( )) ? _back[_back.size( ) - 1 - vIndex] : _front[vIndex - (_back.size( ) - 1)]; }
		inline type& front( )	{ checkInit; return *_front.end( ); }
		inline type& back( )	{ checkInit; return *_back.end( ); }
		inline iter size( )		{ checkInit; return _front.size( ) + _back.size( ); }
		inline bool empty( )	{ checkInit; return size( ) == 0; }
	};
}
