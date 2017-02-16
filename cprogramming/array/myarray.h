#ifndef __MY_ARRAY
#define __MY_ARRAY
#include "myheader.h"

typedef struct _tag_array{
int* *data;
int capacity;
int len;
}array;

array* array_create(int capacity);
void array_destroy(array* arr);
void array_clear(array* arr);
int array_length(array* arr);
int array_capacity(array* arr);
int array_insert(array* arr, int *pdata, int pos);
void * array_get(array* arr, int pos);
void * array_delete(array* arr, int pos);
array* array_resize(array* arr, int capacity);


#endif
