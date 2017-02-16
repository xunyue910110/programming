#include "myheader.h"
#include "myarray.h"

array* array_create(int capacity)
{
    array* arr = (array*)malloc(sizeof(array));
    if (arr == NULL)
    {
        perror("malloc"), exit(-1);
    }
    
    arr->data = malloc(sizeof(void*) * capacity) ;
    arr->capacity = capacity;
    arr->len = 0;
    return arr;
}
void array_destroy(array* arr)
{
    if (arr->data)
    {
        free(arr->data);
        arr->data = NULL;
    }
    if (arr) 
        free(arr);
}

void array_clear(array* arr)
{
    arr->len= 0;
}

int array_length(array* arr)
{
    return arr->len;
}
int array_capacity(array* arr)
{
   return arr->capacity; 
}
int array_insert(array* arr, int *pdata, int pos)
{
    if (arr->len >= arr->capacity)
       array_resize(arr, 2*arr->capacity); 
    if (pos < 0 || pos >arr->len)
        return -1; 
    int i = 0;
    for(i= arr->len-1; i>=pos; i--)
        arr->data[i+1] = arr->data[i];
    arr->data[pos] = (int*)pdata;
    arr->len ++;
    return 0;
}
void* array_get(array* arr, int pos)
{
    if (pos < 0 || pos >=arr->len)
        return NULL; 
    return arr->data[pos]; 
}

void* array_delete(array* arr, int pos)
{
    if (pos < 0 || pos >arr->len)
        return NULL; 
    int* pi = arr->data[pos];
    int i ;
    for(i= pos; i < arr->len-1;  i++)
        arr->data[i] = arr->data[i+1];
    arr->len--;
    return pi;
}

array* array_resize(array* arr, int capacity)
{
    arr->data = realloc(arr->data, sizeof(void*) * capacity);
    arr->capacity = capacity;
    if (arr->data == NULL)
        perror("realloc"), exit(-1); 
    return arr; 
}
