//-------------------------------------------------------------------
// Name      : mgetf.c
// Auther    : QianYiXin
// Version   : 0.2.0
// Update    : 2000.7.24
// Copyright : All Rights Reserved.
//-------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "dta.h"

#define USAGE "\nUSAGE: %s remotehost sourcepath targetpath backuppath\n\n"

int main(int argc,char *argv[])
{
  DTAHCONN hconn;
  struct timeval time0;
  struct timeval time1;
  struct timezone tzp;
  time_t t0;
  time_t t1;
  float dt;
  char result[100][200];
  int i;
  int resultrow;
  char srcfile[200];
  char desfile[200];
  char bakfile[200];
  char rcommand[200];
  int rn;
  int ret;
  DTAFTINFO ftinfo;

  if(argc!=5)
  {
    printf(USAGE,argv[0]);
    return -1;
  }
  if(DTA_INIT())
  {
    printf("init failed\n");
    return -1;
  }
  if(DTA_RESULT((char *)result,200))
  {
    printf("result failed\n");
    return -1;
  }
  for(;;)
  {
    if(ret=DTA_CONN(&hconn,argv[1]))
    {
      printf("connect failed\n");
      if(ret==DTARC_SOCKETCREATE_ERROR||ret==DTARC_SOCKETCONNECT_ERROR)
      {
        sleep(60);
        printf("retry connect\n");
        continue;
      }
      return -1;
    }
    for(;;)
    {
      sprintf(rcommand,"ls -1rtp %s | grep -v '/'",argv[2]);
      if(ret=DTA_RCMD(hconn,rcommand,50,&resultrow))
      {
        if(ret==DTARC_SOCKETREAD_ERROR||ret==DTARC_SOCKETWRITE_ERROR)
          break;
        if(ret!=256)
        {
          printf("rcmd failed\n");
          return -1;
        }
      }
      rn=resultrow;
      for(i=0;i<rn;i++)
      {
        printf("<%s>\n",result[i]);
        sprintf(srcfile,"%s/%s",argv[2],result[i]);
        sprintf(desfile,"%s/%s",argv[3],result[i]);
        time(&t0);
        gettimeofday(&time0,&tzp);
        printf("begin get\n");
        if(ret=DTA_GET(hconn,srcfile,desfile,&ftinfo))
        {
          printf("getfile failed\n");
          if(ret==DTARC_SOCKETREAD_ERROR||ret==DTARC_SOCKETWRITE_ERROR)
            break;
          return -1;
        }
        printf("end get\n");
        time(&t1);
        gettimeofday(&time1,&tzp);

        dt=(double)(time1.tv_sec*1000000+time1.tv_usec-time0.tv_sec*1000000-time0.tv_usec)/1000000;
        printf("%ld %ld %f\n",t0,t1,dt);
        sprintf(rcommand,"mv %s %s",srcfile,argv[4]);
        if(ret=DTA_RCMD(hconn,rcommand,100,&resultrow))
        {
          printf("rcmd failed\n");
          if(ret==DTARC_SOCKETREAD_ERROR||ret==DTARC_SOCKETWRITE_ERROR)
            break;
          return -1;
        }
      }
      sleep(2);
    }
    if(DTA_DISC(hconn))
    {
      printf("disconnect failed\n");
      //return -1;
    }
  }
  if(DTA_EXIT())
  {
    printf("exit failed\n");
    return -1;
  }
}



