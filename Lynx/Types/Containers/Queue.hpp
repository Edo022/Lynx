#pragma once
#include <cstdlib>
#include <cstring>
#include <cstdio>
namespace lynx {
	template<class T>
	struct queue {
		T *top, *base; // top changes, base does not
		size_t size; // size of the queue in bytes

		queue() {
			size = 0;
			base = top = (T*)malloc(1);
		}

		~queue() {
			free(base);		
		}

		// push top
		void push(T data) { 
			size += sizeof(T);
			base = (T*)realloc(base, size);   //Reallocate base
			top = base + (size / sizeof(T)) - 1;             //Update top pointer
			*top = data;                                           //Set value
			// q->top++; //! top is updated after base realloc
		}


		// pop
		T pop() {
			T ret = *top;
			top -= 1;
			size -= sizeof(T);
			//TODO add realloc -sizeof(int) when using memory pool
			return ret;
		}
	};

	template<class T>
	struct deque {
		queue<T> tq, bq;

		
		void pusht(T data) { // push top deque
			tq.push(data);
		}

		void pushb(T data) { // push bottom deque
			bq.push(data);
		}
		
		T popt() {
			if(tq.size > 0) return tq.pop();
			else {
				T ret = bq.base[0];
		
				// shifts rest of Queue down, probabily not the best way to do this
				memmove(bq.base, bq.base+1, bq.size);
				bq.top--;
				return ret;
			}
		}
		
		T popb() {
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