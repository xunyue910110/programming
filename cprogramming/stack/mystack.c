#include "mystack.h"
#include "array/myarray.h"

stack* stack_create(int capacity)
{
   array * arr = array_create(capacity);
   if (arr == NULL) 
    perror(array_create), exit(-1);
    return arr;
}
void stack_destroy(stack* st)
{
    array *arr = (array*) st;
    array_destroy(arr);
}
void stack_clear(stack* st)
{
    array*arr = (array*)st;
    array_clear(arr);
}
int stack_push(stack* st, void *item)
{
    array*arr = (array*)st;
    array_insert(arr, item,  array_length(arr));
}
void* stack_pop(stack* st)
{
    array*arr = (array*)st;
    void *pd = array_delete(arr, array_length(arr)-1);
    return pd;
}
void *stack_top(stack*st)
{
    array*arr = (array*)st;
    void *pd = array_get(arr, array_length(arr));
    return pd;
}
int stack_size(stack*st)
{
    return ((array*)st ) ->len;
}

