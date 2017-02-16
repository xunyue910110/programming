#include <stdio.h>
//#pragma pack(8)
struct s{
    char c1; //0
    int i;   //6
    short s1;   //12
    double d;   //18

    char c2;  //30
};
int main()
{
    char *p ;
    printf("sizeof s: %d\n", sizeof(struct s));
    printf(": %d\n", &((struct s*)(0))->c1);
    printf(": %d\n", &((struct s*)(0))->i);
    printf(": %d\n", &((struct s*)(0))->s1);
    printf(": %d\n", &((struct s*)(0))->d);
    printf(": %d\n", &((struct s*)(0))->c2);
    unsigned int i = -1;
    printf("%d\n", i);
    printf("%u\n", i);
    return 0;
}
