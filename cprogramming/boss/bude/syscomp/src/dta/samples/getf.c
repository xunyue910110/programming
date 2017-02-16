//-------------------------------------------------------------------
// Name      : getf.c
// Auther    : QianYiXin
// Version   : 0.2.0
// Update    : 2000.7.24
// Copyright : All Rights Reserved.
//-------------------------------------------------------------------
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
extern int errno;

#include "dta.h"

#define USAGE "\nUSAGE: %s remotehost sourcefile targetfile\n\n"

int main(int argc,char *argv[])
{
  DTAHCONN hconn;
  DTAHQUEUE hqueue;
  char message[100];
  struct timeval time0;
  struct timeval time1;
  struct timezone tzp;
  time_t t0;
  time_t t1;
  float dt;
  DTAFTINFO ftinfo;
  int conntimeout=10;
  int ret;

  if(argc!=4)
  {
    printf(USAGE,argv[0]);
    return -1;
  }
  time(&t0);
  gettimeofday(&time0,&tzp);
  if(DTA_INIT())
  {
    printf("init failed\n");
    return -1;
  }
  DTA_SETOPT(DTAOPT_CONNTIMEO,&conntimeout,sizeof(int));
  DTA_SETOPT(DTAOPT_RCVTIMEO,&conntimeout,sizeof(int));
  if(DTA_CONN(&hconn,argv[1]))
  {
    printf("connect failed\n");
    return -1;
  }
  printf("begin get\n");
  if(ret = DTA_GET(hconn,argv[2],argv[3],&ftinfo))
  {
    printf("%s\n",DTA_CHKERR(ret));
    printf("stop get\n");
    printf("getfile failed\n");
    printf("errno=%d\n",errno);
    return -1;
  }
  printf("end put\n");
  if(DTA_DISC(hconn))
  {
    printf("disconnect failed\n");
    return -1;
  }
  if(DTA_EXIT())
  {
    printf("exit failed\n");
    return -1;
  }
  time(&t1);
  gettimeofday(&time1,&tzp);

  dt=(double)(time1.tv_sec*1000000+time1.tv_usec-time0.tv_sec*1000000-time0.tv_usec)/1000000;
  printf("%ld %ld %f\n",t0,t1,dt);
}



