#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int flag = 0;
pthread_mutex_t m;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void * thread1(void *p)
{
  for(;;){
    pthread_mutex_lock(&m);
    //int vflag = flag;
    while ( flag == 0)
      pthread_cond_wait(&c, &m);
    pthread_mutex_unlock(&m);
    if (flag ){
      write(1, p, 1);
      sleep(1);
    }
  }
}
int main()
{
  pthread_mutex_init(&m, NULL);
  pthread_t tid1;
  pthread_t tid2;
  pthread_create(&tid1,NULL,thread1, ".");
  pthread_create(&tid2,NULL,thread1, "*");
  for(;;)
  {
     char ch;
     char d;
     //int vflag = 0;
     //c = getchar();
     //
     //printf("char %d\n", c);
     scanf("%c", &ch);
     scanf("%c", &d);

     pthread_mutex_lock(&m);
     if ( ch == 'a')
     {
      flag = 1;
      pthread_cond_signal(&c); 
     }
     else if ( ch == 'b')
     {
       flag = 1;
       pthread_cond_broadcast(&c);
     }
     else
     {
       flag = 0;
     }
     //printf("flag:%d, vflag:%d\n", flag, vflag);
     pthread_mutex_unlock(&m);
     sleep(1);
  }
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
  pthread_mutex_destroy(&m);
}
