#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

sem_t sem;

void *task(void *p)
{
    int i = (int) p;
    sem_wait(&sem);
    printf("thread %d th get the connection.\n", i+1);
    srand(time(0));
    int s = rand()%8;
    sleep(s);
    sem_post(&sem);
    printf("thread %d th release the connectin.\n", i+1);
    
}
int main()
{
    sem_init(&sem, 0, 10);
    pthread_t id;
    int i = 0;
    for (i = 0; i < 15; i++)
    {
        pthread_create(&id, 0, task, (void*)i);
    } 
    sleep(1000);
    sem_destroy(&sem);
}
