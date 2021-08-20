#pragma once
#include "Lynx/Lynx/Types/Containers/Stack.hpp"
// #include "./Stack.hpp"



namespace lnx {
	template<class tType> struct Deque {
		Stack<tType> tq, bq;

		/**
		* @brief Adds an element to the top of the Deque
		* The new element is copy constructed
		* Complexity:
		*     Best:  O(1)
		*     Worst: O(n) [Memory reallocation]
		* @param pElm The element to add
		*/
		inline void pushT(tType& pElm) { // push top deque
			tq.push(pElm);
		}

		/**
		* @brief Adds an element to the bottom of the Deque
		* The new element is copy constructed
		* Complexity:
		*     Best:  O(1)
		*     Worst: O(n) [Memory reallocation]
		* @param pElm The element to add
		*/
		inline void pushB(tType& pElm) { // push bottom deque
			bq.push(pElm);
		}

		/**
		* @brief Removes the top element of the Stack, calls its destructor and returns its value
		* Complexity:
		*     Best:  O(1)
		*     Worst: O(n) //FIXME
		* @return The value of the removed element
		*/
		tType popT() {
			if(tq.size > 0) return tq.pop();
			else {
				tType ret = bq.base[0];

				// shifts rest of Stack down, probabily not the best way to do this //FIXME
				memmove(bq.base, bq.base+1, bq.size);
				bq.top--;
				return ret;
			}
		}

		/**
		* @brief Removes the bottom element of the Stack, calls its destructor and returns its value
		* Complexity:
		*     Best:  O(1)
		*     Worst: O(n) //FIXME
		* @return The value of the removed element
		*/
		tType popB() {
			if(bq.size > 0 ) return bq.pop();
			else {
				tType ret = tq.base[0];
				// same as with poptdq
				memmove(tq.base, tq.base+1, tq.size);
				tq.top--;
				return ret;
			}
		}
	};
}