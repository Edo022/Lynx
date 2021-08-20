#pragma once
#include <cstdlib>
#include <cstring>


namespace lnx {
	template<class T>
	struct Stack {
		T *top, *base; // top changes, base does not
		size_t size; // size of the queue in bytes

		/**
		* @brief Initalized the Stack
		* Complexity:
		*     Best: O(1)
		*     Worst: O(1)
		*/
		Stack() {
			size = 0;
			base = top = (T*)malloc(1);
		}

		/**
		* @brief Destroys the Stack
		* Complexity:
		*     Best: O(1)
		*     Worst: O(1)
		*/
		~Stack() {
			free(base);		
		}


		/**
		* @brief Pushes data to the top of the Stack
		* Complexity:
		*     Best: O(1)
		*     Worst: O(1)
		* @param data The data to be pushed on the Stack
		*/
		void push(T data) { 
			size += sizeof(T);
			base = (T*)realloc(base, size);   //Reallocate base
			top = base + (size / sizeof(T)) - 1;             //Update top pointer
			*top = data;                                           //Set value
			// q->top++; //! top is updated after base realloc
		}


		/**
		* @brief Pops the top of the Stack
		* Complexity:
		*     Best: O(1)
		*     Worst: O(1)
		* @return Data that was popped from the top of the Stack
		*/
		T pop() {
			T ret = *top;
			top -= 1;
			size -= sizeof(T);
			//TODO add realloc -sizeof(int) when using memory pool
			return ret;
		}
	};

	template<class T>
	struct Deque {
		Stack<T> tq, bq;

		/**
		* @brief Pushes data to the top of the Deque
		* Complexity:
		*     Best: O(1)
		*     Worst: O(1)
		* @param data The data to be pushed at the top of the Deque
		*/		
		void pushT(T data) { // push top deque
			tq.push(data);
		}

		/**
		* @brief Pushes data to the bottom of the Deque
		* Complexity:
		*     Best: O(1)
		*     Worst: O(1)
		* @param data The data to be pushed at the bottom of the Deque 
		*/
		void pushB(T data) { // push bottom deque
			bq.push(data);
		}

		/**
		* @brief Pops the top of the Deque
		* Complexity:
		*     Best: O(1)
		*     Worst: O(n)
		* @return Data that was popped from the top of the Deque
		*/		
		T popT() {
			if(tq.size > 0) return tq.pop();
			else {
				T ret = bq.base[0];
		
				// shifts rest of Stack down, probabily not the best way to do this
				memmove(bq.base, bq.base+1, bq.size);
				bq.top--;
				return ret;
			}
		}

		/**
		* @brief Pops the bottom of the Deque
		* Complexity:
		*     Best: O(1)
		*     Worst: O(n)
		* @return Data that was popped from the bottom of the Deque
		*/	
		T popB() {
			if(bq.size > 0 ) return bq.pop();
			else {
				T ret = tq.base[0];
				// same as with poptdq
				memmove(tq.base, tq.base+1, tq.size);
				tq.top--;
				return ret;
			}
		}

	};

}