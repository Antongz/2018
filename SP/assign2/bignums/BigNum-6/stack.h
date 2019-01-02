#ifndef __STACK_H_
#define __STACK_H__ 1

#include "bn.h"

typedef struct stackElement* stEl;
typedef struct stackStore* st;  

// initializes the stack
// if int m is set to 1 it will also free space for bn_t elements when popStack is called.
// This can be used in conjunction with a second stack B (m=1) in which elements popped from stack A (m=0)
// are pushed. This is so if elements are duplicated and have the same memory address popping one from
// stack A would not also free the space for the other.]
// returns NULL if memory for stack cannot be allocated 
st initStack(int m);

// pushes elements onto the stack
// returns 1 if can't push element i on to stack
int pushStack(st myStack, bn_t i);

// returns the element on the top of the stack
// returns NULL if no elements on stack
bn_t topStack(st myStack);

// removes element on top of the stack
// returns 1 if nothing to pop
int popStack(st myStack);

// duplicates the top element of the stack and
// pushes it on to the stack
// returns 1 if nothing to duplicate or cant create new element
int dupStack(st myStack);

// prints the contents of top element of the stack to console
// returns 1 if unsuccessful or nothing to print
int printStack(st myStack);

// swaps the top two elements of the stack
// returns 1 if two elements are not present to swap or swap cant be performed
int swapStack(st myStack);

// prints contents of the stack one line at a time starting from the top
// returns 1 if not successful
int dumpStack(st myStack);

// clears the content of the stack
int clearStack(st myStack);

// frees resources for stack head
void freeStack(st myStack);

#endif // __STACK_H__
