#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void * start(void *arg)
{
  *(int*)arg = 200;
  for(int i = 0; i < 20; i++)
  {
    write(1,".", 1);
    sleep(1);
  }

  return (void*)8888;
}
int main()
{
  pthread_t tid;
  int x = 100;
  int ret = pthread_create(&tid, NULL, start, (void*)(&x));
  if ( ret != 0)
  {
    int errno = ret; printf("%s\n",strerror(errno));exit(-1);
  }
  for(int i = 0;i <10; i++)
  {
    write(1, "*", 1); sleep(1);
  }
  printf("%d\n", x);
  //void* threadret;
  int result;
  pthread_join(tid, (void*)&result);
  printf("threadret=%d\n", result);
  return 0;
}
