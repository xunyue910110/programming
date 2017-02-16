#ifndef __MY_STACK
#define __MY_STACK
#include "myheader.h"
#include "array/myarray.h"
typedef void stack;
stack* stack_create(int capacity);
void stack_destory(stack* st);
void stack_clear(stack* st);
int stack_push(stack*, void *item);
void* stack_pop(stack*);
void *stack_top(stack*);
int stack_size(stack*);

#endif
