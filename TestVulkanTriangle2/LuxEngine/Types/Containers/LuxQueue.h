
#pragma once
#include "LuxDynArray.h"




//A queue with dynamic size
namespace lux {
	template<class type, class alloc = uint32> struct Queue {
		lux::DynArray<type, alloc> _front, _back;

		inline Queue() : _front{ 0xFF }, _back{ 0xFF } { }


		//Adds an element at the back of the queue
		inline void pushBack(const type& vElement) { _back.add(vElement); }
		//Adds an element at the front of the queue
		inline void pushFront(const type& vElement) { _front.add(vElement); }
		//Removes an element from the back of the queue
		inline void popBack(const type& vElement) { _back.resize(_back.__lp_size - 1); }
		//Removes an element from the front of the queue
		inline void popFront() { _front.resize(_front.__lp_size - 1); }


		inline type& operator [](const alloc vIndex) { return (vIndex < _back.__lp_size) ? _back[_back.__lp_size - 1 - vIndex] : _front[vIndex - (_back.__lp_size - 1)]; }
		inline type& front() { return _front.last(); }
		inline type& back() { return _back.last(); }
		inline alloc size() { return _front.size() + _back.size(); }
		inline bool empty() { return size() == 0; }
	};
}
