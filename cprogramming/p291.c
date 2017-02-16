#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void *start(void *arg)
{
    int s = pthread_join(pthread_self(), NULL);
    if ( s != 0)
    {
        errno=s;
        perror("pthread_join");
        pthread_exit((void*)-1);    
    }
    return NULL;
}
int main()
{
    pthread_t t1;
    int val;
    int s = pthread_create(&t1, NULL, start, NULL);
    if (s != 0)
    {
        perror("pthread_create");
        exit(-1);
    }
    if (! pthread_equal( t1, pthread_self()))
    {
        s = pthread_join(t1, (void **) &t1);
        if ( s != 0)
        {
        strerror(s);
        pthread_exit((void*)-1);    
        }
        printf("retval: %d\n", t1);
    } 
    return 0;
}
