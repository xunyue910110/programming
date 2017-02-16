#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

char *data[5];
int size =  0;
sem_t sem;
void *task(void *p)
{
    sem_wait(&sem);
    data[size] = (char*)p;
    usleep(10000);
    size++;
    sem_post(&sem);
}

int main()
{
    data[size] = "liubei"; size++;
    pthread_t id1, id2;

    sem_init(&sem, 0, 1);
    pthread_create(&id1, 0, task, "zhangfei");
    pthread_create(&id2, 0, task, "guanyu");
    pthread_join(id1, 0); pthread_join(id2, 0);
    sem_destroy(&sem);
    int i ;
    printf("size = %d\n", size);
    for(i = 0; i < size ; i++)
    printf("%s\n", data[i]);
}
