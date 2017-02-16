#include <stdio.h>
#include <string.h>
#include <pthread.h>

void * task(void *p)
{
    int i ; 
    pthread_t pid = pthread_self();
    for (i = 0 ; i< 50; i ++) printf("thread id %lu: %d\n",pid, i);
}

int main()

{
    pthread_t id;
    int res = pthread_create( &id, 0, task, 0);
    if (res > 0) printf("%s\n", strerror(res));
    pthread_t mid;
    mid = pthread_self();
    printf("main thread id: %lu\n", mid);
    pthread_t pid = pthread_self();
    int i;
    for (i = 0 ; i< 50; i ++) printf("main thread id %lu: %d\n",pid, i);
    pthread_join(id, 0);
}
