#include <stdio.h>
#include <string.h>
#include <pthread.h>

void * task(void *p)
{
    double *pd  = (double *)p ; 
    printf("s = %lf\n", 3.14*(*pd)*(*pd));
}

int main()

{
    pthread_t id;
    double r = 100;
    int res = pthread_create( &id, 0, task, &r);
    if (res > 0) printf("%s\n", strerror(res));
    pthread_join(id, 0);
}
