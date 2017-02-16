#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void * start(void *arg)
{
  sleep(10);
  printf("in start\n");
  return NULL;
}
int main()
{
  pthread_t pid;
  pthread_create(&pid, NULL, start, NULL);
  printf("in main\n");
  //pthread_exit(NULL);
  return 0;
}
