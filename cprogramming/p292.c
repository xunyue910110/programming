#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void *start(void *arg)
{
    int *pibuf = (int*) arg;
    printf ( "%d\n", *pibuf);
    if ( pibuf)
        free(pibuf);
    sleep(2);
    return NULL;
}

int main()


{
    pthread_t tid;
    int s ;
    int *val = malloc(sizeof(int));
    *val = 1;
    s = pthread_create(&tid, NULL, start, (void*)val);
    if ( s != 0)
    {
        errno = s;
        perror("create");
        pthread_exit((void*) -1);
    }
    pthread_exit(NULL);
}
