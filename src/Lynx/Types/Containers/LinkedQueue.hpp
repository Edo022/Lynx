#pragma once
#include <cstdlib>

template<class T>
struct Element {
    T value;
    Element<T> *next, *previous;
};


template<class T>
struct LiQueue {
    Element<T> *base, *last;

    LiQueue() {
        last = base = (Element<T>*)malloc(sizeof(Element<T>)); // Allocate first element
        last->previous = last->next = NULL;
        base->previous = base->next = NULL; 
    }
    
    ~LiQueue() {
        while(last != NULL) {
            free(last);
            last = last->previous;
        }
    }
    
    void push(T data) {
        last->value = data; // puts the data in the last element
        last->next = (Element<T>*)malloc(sizeof(Element<T>)); // allocates new last element
        last->next->previous = last; // sets the old last element as the previous element of the new last element
        last->next->next = NULL;
        last = last->next; // moves the last pointer to the new last element
    }

    T pop() {
        T ret = base->value; // saves the value at the base
        base = base->next; // moves the base to the next value
        free(base->previous); // frees the previous value of the base
        base->previous = NULL; // sets the freed value to NULL
        return ret; // returns the saved value
        
    } 

};