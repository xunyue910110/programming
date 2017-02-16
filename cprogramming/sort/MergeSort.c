#include "myheader.h"

void MergeSort(int array[] , int n )
{
    if (n == 1)
       return;
    if (n == 2)
    {
        if (array[0] > array[1]) 
            swap(array[0], array[1]);
    }
    MergeSort(array, n/2);
    MergeSort(array+n/2, n-n/2);
    Combine(array, n);
}

void Combine(int array[], int n)
{
    int i =0; 
    int j =0;
    int k = 0;
    int *p = (int *) malloc(sizeof(int) *n);
    int *pa = p;
    while ( i < n/2 && j < n-n/2)
    {
        int t1 = *(array+i);
        int t2 = *( array+n/2 +j);
        if (t1 > t2)
        {
            pa[k] = t2;   
            j++;
            k++;
        }
        else 
        {
           pa[k] = t1; 
            i++;
            k++;
        }
    }
    while ( i < n/2) 
    {
        pa[k++] = *(array+i) ;
        i++;
    }
    while ( j < (n - n/2))
    {
        pa[k++] = *(array+n/2+j); j++;
    }
    memcpy(array, p, sizeof(int)*n);           
    free(p); 
}
