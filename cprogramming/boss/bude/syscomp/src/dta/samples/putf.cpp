#include "ft.h"
#include <sys/time.h>
#include <stdio.h>

#define USAGE "\nUSAGE: %s remotehost sourcefile targetfile\n\n"

int main(int argc,char *argv[])
{
  CFileTransf FT;
  char message[100];
  struct timeval time0;
  struct timeval time1;
  struct timezone tzp;
  time_t t0;
  time_t t1;
  float dt;
  DTAFTINFO ftinfo;

  if(argc!=4)
  {
    printf(USAGE,argv[0]);
    return -1;
  }
  time(&t0);
  gettimeofday(&time0,&tzp);
  if(FT.conn(argv[1]))
  {
    printf("connect failed\n");
    return -1;
  }
  printf("begin put\n");
  if(FT.putfile(argv[2],argv[3],&ftinfo))
  {
    printf("stop put\n");
    printf("putfile failed\n");
    fflush(stdout);
    return -1;
  }
  printf("end put\n");
  if(FT.disconn())
  {
    printf("disconnect failed\n");
    return -1;
  }
  time(&t1);
  gettimeofday(&time1,&tzp);

  dt=(double)(time1.tv_sec*1000000+time1.tv_usec-time0.tv_sec*1000000-time0.tv_usec)/1000000;
  printf("%ld %ld %f\n",t0,t1,dt);
}


