#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>

int main()
{
    key_t key;
    char i;
    for ( i = 'a'; i < 'd'; ++i)
    {
        key = ftok(".", i);
        printf("proj = %x, key = %x,  MSB = %c\n", i, key, (key>>24));
    }
    return 0;
}
