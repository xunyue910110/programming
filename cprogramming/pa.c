#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int a = 0;

void *thread1_func(void *p_arg)
{
  while(1)
  {
    a++;
    sleep(10);
  }
}

int main()
{
  pthread_t t1;
  pthread_create(&t1, NULL, thread1_func, "Thread 1");
  sleep(1000);
  return 0;
}
