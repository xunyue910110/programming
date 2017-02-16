#include "myheader.h"

int partition(int array[], int low, int high)
{
    int pv = array[low];
    while( low < high)
    {
        while( (low < high) && (array[high] >= pv))
        {
            high--;
        } 
        swap(array[low], array[high]);
        while( (low <high) && (array[low] <= pv))
        {
            low++; 
        }
        swap(array[low], array[high]);
    }
    return low;
}
/* get from the alorigthm video */
int partition2(int array[], int p, int q)
{
   int pv = array[p];
    int i = p;
    int j = 0;
    for(j=p+1; j<=q; j++)  
    {
        if (array[j] <= pv)
        {
            i++;
            swap(array[i], array[j]);
        }
    } 
    swap(array[p],array[i]);
    return i;
}
void QuickSort(int array[], int n)
{
    QuickSort1(array, 0, n-1);
}

void QuickSort1(int array[], int low, int high)
{
    if (low < high)
    {
        int pivot = partition2(array, low, high);
        QuickSort1(array, low, pivot -1);
        QuickSort1(array, pivot+1, high);
    }
}

