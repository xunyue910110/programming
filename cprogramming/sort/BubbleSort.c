#include "myheader.h"

void BubbleSort(int array[] , int n)
{
    int i; 
    int j;
        
    for (i = 0; i < n-1; i++)
    {
        int sorted = 1;
        for (j = n-1; j > i; j--)
        {
            if (array[j] < array[j-1])
            {
                swap(array[j], array[j-1]);
                sorted = 0;
            }
        }
        if (sorted == 1)
        break;
    }
}
