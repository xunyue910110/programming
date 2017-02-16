#include <pthread.h>
#include <stdlib.h>

void *start_routine(void *arg){
  for (;;)
  {
    int a = 0 ;
    a++;
  }
  return NULL;
}

int main(){
  pthread_t tid[10];

  int i;
  for (i = 0; i < 10; i++)
    pthread_create(&tid[i], NULL, start_routine, NULL);

  int j;
  for (j = 0; j < 10; j++)
    pthread_join(tid[j], NULL);
  return 0;
}
