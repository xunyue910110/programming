//-------------------------------------------------------------------
// Name      : writeq.c
// Version   : 0.2.4
// Update    : 2000.6.22
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : A example program to write message into queue.
//-------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "dta.h"

#define USAGE "\nUSAGE: %s dqueuename message msglen priority\n\n"

int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  int ret;

  if(argc!=5)
  {
    printf(USAGE,argv[0]);
    return -1;
  }

  if(ret=DTA_INIT())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_OPENQ(&hqueue,argv[1]))
  {
    printf("DTA_OPENQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_PUTMSG(hqueue,argv[2],atol(argv[3]),atol(argv[4])))
  {
    printf("DTA_PUTMSG(): %s",DTA_CHKERR(ret));
    DTA_CLOSEQ(hqueue);
    return -1;
  }
  if(ret=DTA_CLOSEQ(hqueue))
  {
    printf("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_EXIT())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}



