#include "myheader.h"
#include "myarray.h"

typedef struct _teacher
{
char name[20];
char password[20];
}teacher;

int main()
{
    
    array* arr = array_create(2);
    //array_clear(arr);
    printf("array length: %d\n", array_length(arr));
    teacher t1, t2, t3;
    teacher t4, t5, t6;
    strcpy(t1.name, "zj1"); strcpy(t1.password, "zj1");
    strcpy(t2.name, "zj2"); strcpy(t2.password, "zj2");
    strcpy(t3.name, "zj3"); strcpy(t3.password, "zj3");
    strcpy(t4.name, "zj4"); strcpy(t4.password, "zj4");
    strcpy(t5.name, "zj5"); strcpy(t5.password, "zj5");
    strcpy(t6.name, "zj6"); strcpy(t6.password, "zj6");
    array_insert(arr, (void*)&t1,array_length(arr)) ;
    array_insert(arr, (void*)&t2, array_length(arr));
    array_insert(arr, (void*)&t3, array_length(arr));
    array_insert(arr, (void*)&t4, array_length(arr));
    array_insert(arr, (void*)&t5, array_length(arr));
    array_insert(arr, (void*)&t6, array_length(arr));
    
    int i = 0; 
    array *pd = NULL;
    while ( i < array_length(arr))
    {
        pd = array_get(arr, i); 
        printf("name: %s, password: %s\n", ((teacher*)pd)->name, ((teacher*)pd)->password);
        i++;
    }
    printf("--------------\n");
    printf("array length: %d\n", array_length(arr));
    i = 0; 
    while ( i < array_length(arr))
    {
        pd = array_delete(arr,i);
        printf("name: %s, password: %s\n", ((teacher*)pd)->name, ((teacher*)pd)->password);
    } 
    array_destroy(arr);
    return 0;
}
