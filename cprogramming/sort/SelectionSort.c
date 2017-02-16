#include "myheader.h"
void SelectionSort(int array[] , int n)
{
    int i;
    int j;
    
    for ( i=0; i<n-1; i++) // loop n-1 times
    {
        int minindex = i;
        for (j = i+1; j < n; j++) //index <- i+1 .. n-1
        {
           if (array[j] < array[minindex] )
            {
                minindex = j;
            } 
            if (minindex != i)
            {
                swap(array[minindex], array[i]);
            }
        }
    } //
}
