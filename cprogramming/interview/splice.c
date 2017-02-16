#include <stdio.h>
void splice(int *a, int n)
{
    int key = 0;
    int j = 0;
    int i = 0;
    for (i =0; i <n ; i++)
    {
       key = a[i];
       
        if (key < 0)
        {
           j=i-1;
            while(j >=0 && a[j] >=0)
            {
                a[j+1] = a[j];
                j--;
            } 
            a[j+1] = key;
        } 
    }
}

int main()
{
    //int a[] = {1,7,-5,9, -12, 5};
    int a[] = {-1,7,-5,9, -12, 5};
    int i = 0;
    splice(a, 6);    
    for(i = 0; i < 6; i++)
    printf("%d ", a[i]);
    return 0;
}
