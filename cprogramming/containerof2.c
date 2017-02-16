#include <stdio.h>
#include <stdlib.h>

#define offsetof(TYPE,MEMBER) ((size_t)&((TYPE*)0)->MEMBER)
#define container_of(ptr,type,member) ({\
    typeof(((type*)0)->member)*__mptr = (ptr);\
    (type*)((char*)__mptr-offsetof(type,member));\
})

struct student{
char num[5];
char name[10];
int age;
};

int main()
{
    struct student stu = {"001", "zhange", 10};
    struct student *s = container_of(&(stu.name), struct student, name);
    printf("name=%s, sex=%s, age=%d\n", 
        s->name, s->num, s->age);
    return 0;
}
