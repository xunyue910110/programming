#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int ga;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* start_routine(void *arg)
{
    pthread_mutex_lock(&mutex);
   printf("%d", ga); 
    fflush(stdout);
    sleep(1);
   ga++; 
    pthread_mutex_unlock(&mutex);
}
int main()
{
    ga = 1;
    pthread_t tid1; 
    pthread_t tid2; 
    pthread_create(&tid1, NULL, start_routine, NULL);
    pthread_create(&tid2, NULL, start_routine, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
