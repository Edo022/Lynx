#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _queue {
    int *top, *base; // top changes, base does not
    size_t size; // size of the queue in bytes
} queue;

typedef struct _deque {
	queue *tq, *bq;
} deque;


// initialize a queue struct by setting it's size and allocating the memory
void initq(queue* q) {
    q->size = 0;
    q->base = q->top = (int*)malloc(1);
}

void initdq(deque* dq) {
	dq->tq = (queue*)malloc(sizeof(queue));
	dq->bq = (queue*)malloc(sizeof(queue));
	initq(dq->tq);
	initq(dq->bq);
}

void freedq(deque* dq) {
	free(dq->tq);
	free(dq->bq);
}

// push top
void pushq(queue* q, int data) { 
    q->size += sizeof(int);
    q->base = (int*)realloc(q->base, q->size);   //Reallocate base
    q->top = q->base + (q->size / sizeof(int)) - 1;             //Update top pointer
    *(q->top) = data;                                           //Set value
    // q->top++; //! top is updated after base realloc
}

// pop
int popq(queue* q) {
    int top = *(q->top);
    q->top -= sizeof(int);
    q->size -= sizeof(int);
    //TODO add realloc -sizeof(int) when using memory pool
    return top;
}

void pushtdq(deque* q, int data) { // push top deque
	pushq(q->tq, data);
}

void pushbdq(deque* q, int data) { // push bottom deque
	pushq(q->bq, data);
}

int poptdq(deque* q) {
	if(q->tq->size > 0) return popq(q->tq);
	else {
		int ret = q->bq->base[0];

		// shifts rest of Queue down, probabily not the best way to do this
		memmove(q->bq->base, q->bq->base+1, q->bq->size);
		q->bq->top--;
		return ret;
	}
}

int popbdq(deque* q) {
	if(q->bq->size > 0 ) return popq(q->bq);
	else {
		int ret = q->tq->base[0];
		// same as with poptdq
		memmove(q->tq->base, q->tq->base+1, q->tq->size);
		q->tq->top--;
		return ret;
	}
}
