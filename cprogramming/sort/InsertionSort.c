
void InsertionSort(int array[], int n)
{
    int i;
    int j;
    
    for (i = 1; i<n; i++)
    {
        int k = i;
        int temp = array[i];
        for (j = i-1; (j>=0) && array[j]>temp; j--)
        {
            array[j+1] = array[j];
        }
        array[j+1] = temp;
    }
}
