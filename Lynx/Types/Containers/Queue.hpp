#pragma once
#include <stdio.h>
#include <stdlib.h>

//typedef struct _queue {
//    int *top, *base; // top and bottom change, base does not
//    size_t size, used; // size of the queue and bytes used to check if it's full
//} queue;


//// initialize a queue struct by setting it's size and allocating the memory
//void init(queue* q, size_t size) {
//    q->size = size;
//    q->used = 0;
//    q->base = q->top = malloc(size);
//}


//// expand q by chunk bytes
//void expand(queue* q, size_t chunk) {
//    q->size += chunk;
//    int offset = q->top - q->base;
//    q->base = realloc(q->base, q->size);
//    q->top = q->base + offset;
//}

//// push top
//void push(queue* q, int data, size_t chunk) { // chunk is how much memory to add if reallocating
//    q->used += sizeof(int);
//    if(q->used > q->size) expand(q, chunk);
//    *(q->top) = data;
//    q->top += sizeof(int); // change this size based on data type
//}

//// pop top
//int pop(queue* q) {
//    q->top -= sizeof(int);
//    return *(q->top);
//}


//// get from base
//int getfb(queue* q, int index) {
//    return q->base[index * sizeof(int)];
//}








typedef struct _queue {
    int *top, *base; // top and bottom change, base does not
    size_t size; // size of the queue in bytes
} queue;


// initialize a queue struct by setting it's size and allocating the memory
void init(queue* q) {
    q->size = 0;
    q->base = q->top = (int*)malloc(1);
}


// // expand q by chunk bytes
// void expand(queue* q, size_t chunk) {
//     q->size += chunk;
//     int offset = q->top - q->base;
//     q->base = (int*)realloc(q->base, q->size);
//     q->top = q->base + (offset / sizeof(int));
// }

// push top
void push(queue* q, int data) { // chunk is how much memory to add if reallocating
    q->size += sizeof(int);
    q->base = (int*)realloc(q->base, q->size);   //Reallocate base
    q->top = q->base + (q->size / sizeof(int)) - 1;             //Update top pointer
    *(q->top) = data;                                           //Set value
    // q->top++; //! top is updated after base realloc
}

// pop top
int pop(queue* q) {
    int top = *(q->top);
    q->top--;
    //TODO add realloc -sizeof(int) when using memory pool
    return top;
}


// get from base
int getfb(queue* q, int index) {
    return q->base[index * sizeof(int)];
}

