#include "myheader.h"
#include "mystack.h"

typedef struct _teacher
{
char name[20];
char password[20];
}teacher;

int main()
{
    
    stack* st = stack_create(2);
    //stack_clear(st);
    printf("stack size: %d\n", stack_size(st));
    teacher t1, t2, t3;
    teacher t4, t5, t6;
    strcpy(t1.name, "zj1"); strcpy(t1.password, "zj1");
    strcpy(t2.name, "zj2"); strcpy(t2.password, "zj2");
    strcpy(t3.name, "zj3"); strcpy(t3.password, "zj3");
    strcpy(t4.name, "zj4"); strcpy(t4.password, "zj4");
    strcpy(t5.name, "zj5"); strcpy(t5.password, "zj5");
    strcpy(t6.name, "zj6"); strcpy(t6.password, "zj6");
    stack_push(st, (void*)&t1);
    stack_push(st, (void*)&t2);
    stack_push(st, (void*)&t3);
    stack_push(st, (void*)&t4);
    stack_push(st, (void*)&t5);
    stack_push(st, (void*)&t6);
    
    int i = 0; 
    void *pd = NULL;
    while ( 0< stack_size(st))
    {
        pd = stack_pop(st);
        printf("name: %s, password: %s\n", ((teacher*)pd)->name, ((teacher*)pd)->password);
    }
    printf("--------------\n");
    printf("stack size: %d\n", stack_size(st));
    stack_destroy(st);
    return 0;
}
