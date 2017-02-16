#include "myheader.h"
void ShellSort(int array[], int n)
{
    int i; 
    int j ;
    int gap;
    int k; 
    do {
    gap = gap/3 + 1;
    for ( i = gap; i <n; i+=gap)
    {
        k = i;
        int temp = array[k];
        for (j = i-gap; (j>=0) && (array[j] > temp); j--)
        {
            array[j+gap] = array[j];
        }
        array[j+1] =  temp;
    }
    }while( gap > 1);
}
