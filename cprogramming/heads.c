#include <stdlib.h>
#include <stdio.h>
#include <time.h>
int heads(){
int x = rand();
printf("%d\n", x);
return x  < RAND_MAX/2;}
int main(int argc, char*argv[])
{
    int i, j , cnt;
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    srand(time(NULL));
    int *f = (int *) malloc((N+1)*sizeof(int));
    for (j = 0; j < N+1; j++)
        f[j] = 0;
    for (i = 0; i<M; i++, f[cnt]++)
        for(cnt=0,j=0; j<=N; j++)
            if (heads()) cnt++;
    for(j=0; j<N+1; j++)
    {
        printf("%2d ", j);
        for (i = 0; i <f[j]; i+=10) printf("*");
        printf("\n");
    }
    return 0;
}
