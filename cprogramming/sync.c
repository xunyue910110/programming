#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

char *data[5];
int size =  0;
pthread_mutex_t mutex;

void *task(void *p)
{
    pthread_mutex_lock(&mutex);
    data[size] = (char*)p;
    usleep(10000);
    size++;
    pthread_mutex_unlock(&mutex);
}

int main()
{
    data[size] = "liubei"; size++;
    pthread_t id1, id2;

    pthread_mutex_init(&mutex, 0);
    pthread_create(&id1, 0, task, "zhangfei");
    pthread_create(&id2, 0, task, "guanyu");
    pthread_join(id1, 0); pthread_join(id2, 0);
    int i ;
    printf("size = %d\n", size);
    for(i = 0; i < size ; i++)
    printf("%s\n", data[i]);
}
