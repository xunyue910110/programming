#include "tlpi_hdr.h"

static int glob = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct {
    int loops;
    pthread_t tid;
} *thread;
static void *
threadFunc(void *arg){
    int loops = *(int* ) arg;
    int loc, j;
    int s;
    pthread_t tid = thread_self();
    
    for (j = 0 ; j < loops; j++){
        /*
        loc = glob;
        loc++;
        glob = loc;
        */
        s = pthread_mutex_lock(&mutex);
        if (s != 0) 
            errExitEN(s, "pthread_mutex_lock");
        glob++;
        printf("%d\n", glob);
        printf("tid: %lu\n", tid);
        
        s =pthread_mutex_unlock(&mutex);
        if (s != 0) 
            errExitEN(s, "pthread_mutex_unlock");
    }
    return NULL;
}

int main(int argc, char*argv[])
{

    pthread_t t1, t2;
    int loops, s;

    if ( argc != 2)
        loops = 10000000;
    else 
        loops = atoi(argv[1]); 
   
    pthread = calloc( 2 * sizeof(*thread)); 
    if ( pthread == NULL)
        errExit("calloc");
    int idx = 0;
    for (idx =0; idx < 2; idx++)
    {
    s = pthread_create(&t1, NULL, threadFunc, &loops);
    if ( s!= 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &loops);
    if ( s!= 0)
        errExitEN(s, "pthread_create");
    }
    s = pthread_join(t1, NULL);
    if ( s!=0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if ( s!=0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
