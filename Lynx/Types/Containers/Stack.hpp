#pragma once
#include <cstdlib>
#include <cstring>
//



namespace lnx {
	template<class tType> struct Stack {
		tType *top, *base;	//Top changes, base does not
		size_t size;	//Size of the stack in bytes


		/**
		* @brief Initalizes the Stack //TODO construct non trivial types
		* Complexity: O(1)
		*/
		Stack() {
			size = 0;
			base = top = (tType*)malloc(1);
		}

		/**
		* @brief Destroys the Stack //TODO destroy non trivial types
		* Complexity: O(1)
		*/
		~Stack() {
			free(base);
		}


		/**
		* @brief Adds an element to the top of the Stack
		* The new element is copy constructed
		* Complexity:
		*     Best:  O(1)
		*     Worst: O(n) [Memory reallocation]
		* @param pElm The element to add
		*/
		void push(tType& pElm) {
			size += sizeof(tType);
			base = (tType*)realloc(base, size);				//Reallocate base
			top = base + (size / sizeof(tType)) - 1;		//Update top pointer
			*top = pElm;									//Set value
			// q->top++; //! top is updated after base realloc
		}


		/**
		* @brief Removes the top element of the Stack, calls its destructor and returns its value
		* Complexity: O(1)
		* @return The value of the removed element
		*/
		tType pop() {
			tType ret = *top;
			top->~tType();
			top -= 1;
			size -= sizeof(tType);
			//TODO use realloc -sizeof(int) when using memory pool
			return ret;
		}
	};
}