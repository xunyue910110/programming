#include <stdio.h>
#include <pthread.h>
#include <string.h>

void *task(void *p)
{
    int x = 55;
//    return (void*)x;
    pthread_exit((void*)x);
}

int main()
{
    pthread_t id;
    pthread_create(&id, 0, task, 0);
    
    int ret;
    pthread_join(id, (void **)&ret);
    printf("ret=%d\n", ret);
}
