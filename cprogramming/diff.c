#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "num.h"
/*
typedef float Number;
Number randNum(){
//return rand()%99;
float f = 0.0;
f = 1.0 * rand() / RAND_MAX;
printf( "f:%f\n", f);
int x = f;
return f;
}
*/
int main(int argc, char*argv[])
{

    int i, n = atoi(argv[1]);
    float m1 = 0.0;
    float m2 = 0.0;
    Number x;
    srand(time(NULL));
    printf("%d\n", RAND_MAX);
    for (i = 0; i < n; i++)
    {
        x = randNum();
printf("x: %f\n", x);
        m1 += ((float) x) / n;
        m2 += ((float) x*x)/n;
    }
    printf("    Average: %f\n", m1);
    printf("Std. deviation: %f\n", sqrt(m2-m1*m1)); 
    return 0;
}
