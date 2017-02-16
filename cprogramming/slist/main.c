#include "myheader.h"
#include "myslist.h"
typedef struct _teacher
{
slist node;
char name[20];
char password[20];
}teacher;

int main()
{
    
    slist* l1 = slist_create();
    slist_clear2(l1);
    printf("slist length: %d\n", slist_length(l1));
    teacher t1, t2, t3;
    strcpy(t1.name, "zj"); strcpy(t1.password, "zj");
    strcpy(t2.name, "zj2"); strcpy(t2.password, "zj2");
    strcpy(t3.name, "zj3"); strcpy(t3.password, "zj3");
    slist_insert(l1, (slist*)&t1,0) ;
    slist_insert(l1, (slist*)&t2, 0);
    slist_insert(l1, (slist*)&t3, 0);
    //slist_delete(l1, 0);
    
    int i = 0; 
    slist *pd = NULL;
    while ( i < slist_length(l1))
    {
        pd = slist_get(l1, i); 
        printf("name: %s, password: %s\n", ((teacher*)pd)->name, ((teacher*)pd)->password);
        i++;
    }
    printf("--------------\n");
    printf("slist length: %d\n", slist_length(l1));
    i = 0; 
    while ( i < slist_length(l1))
    {
        pd = slist_delete(l1,i);
        printf("name: %s, password: %s\n", ((teacher*)pd)->name, ((teacher*)pd)->password);
    } 
    slist_destroy2(l1);
    return 0;
}
