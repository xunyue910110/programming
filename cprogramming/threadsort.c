#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
sem_t s1;
sem_t s2;

void *func(void *arg)
{
  int *a = (int*)arg;
  int i;
  for (i = 0;i<10; i++)
  {
    sem_wait(&s1);
    int t = a[i];
    int j;
    for (j = i; j > 0 && t < a[j-1]; j--)
      a[j] = a[j-1];
    a[j] = t; 
    sem_post(&s2);
  }
  return NULL;
}
int main()
{
  int a[10];
  sem_init(&s1, 0, 0);
  sem_init(&s2, 0, 0);

  pthread_t tid;
  pthread_create(&tid, NULL, func, a);
  int i ;
  for (i = 0;i < 10; i++)
  {
    printf("请输入第%d个数据:", i+1);
    scanf("%d", &a[i]);
    sem_post(&s1);
    sem_wait(&s2);
    int j =0;
    for (;j <=i;j++)
    printf("%d ", a[j]);
    printf("\n");
    sem_destroy(&s1);
    sem_destroy(&s2);
  }
  return 0;
}
