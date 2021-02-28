#pragma once
#include "LuxEngine/Types/Containers/RtArray.hpp"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"



//TODO automatic back to front and viceversa in queues
//TODO FIX QUEUES

//A queue with dynamic count
//DEPRECATED
namespace lux {
	template<class type, class iter = uint32> struct Queue {
		genInitCheck;
	private:
	public:
		lux::RtArray<type, iter> _front, _back;

		inline Queue() : _front(), _back() {}

		inline void clear() {
			_front.clear();
			_back.clear();
		}


		//Adds an element at the back of the queue
		inline void pushBack(const type& vElement)	{ checkInit(); _back.add(vElement); }
		//Adds an element at the front of the queue
		inline void pushFront(const type& vElement) { checkInit(); _front.add(vElement); }
		//Removes an element from the back of the queue
		inline void popBack(const type& vElement)	{ checkInit(); _back.resize(_back.count() - 1); }
		//Removes an element from the front of the queue
		inline void popFront()						{ checkInit(); _front.resize(_front.count() - 1); }


		inline type& operator[](const iter vIndex) { checkInit(); return (vIndex < _back.count()) ? _back[_back.count() - 1 - vIndex] : _front[vIndex - (_back.count() - 1)]; }
		inline type& front()	{ checkInit(); return *_front.end(); }
		inline type& back()	{ checkInit(); return *_back.end(); }
		inline iter size()		{ checkInit(); return _front.count() + _back.count(); }
		inline bool empty()	{ checkInit(); return size() == 0; }
	};
}
