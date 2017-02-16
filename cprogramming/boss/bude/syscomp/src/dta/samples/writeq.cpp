#include "mt.h"
#include <stdio.h>
#include <stdlib.h>

#define USAGE "\nUSAGE: %s dqueuename message msglen priority\n\n"

int main(int argc,char *argv[])
{
  CMsgQueue MQ;
  int ret;

  if(argc!=5)
  {
    printf(USAGE,argv[0]);
    return -1;
  }

  if(ret=MQ.init())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=MQ.open(argv[1]))
  {
    printf("DTA_OPENQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=MQ.put(argv[2],atol(argv[3]),atol(argv[4])))
  {
    printf("DTA_PUTMSG(): %s",DTA_CHKERR(ret));
    MQ.close();
    return -1;
  }
  if(ret=MQ.close())
  {
    printf("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=MQ.end())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}

