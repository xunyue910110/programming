#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


void * start(void *arg)
{
  int *arr = (int*)arg;
  int cnt = 100;
  for(int i =0;i <20;i++) 
  {
   arr[i] = cnt++; 
    sleep(1);
  }
  return NULL;
}
int main()
{
  int a[20] ;
  pthread_t tid;
  pthread_create(&tid, NULL, start, (void*)a);
  //sleep(1);
  for(int i = 0; i< 20; i++)
  {
  //  write(1, a+i, 4); 
    printf("%d ", a[i]); 
    fflush(stdout);
    sleep(1);
   // fprintf(stderr, "%d ", a[i]);
  }
  pthread_join(tid, NULL);
  return 0;
}
