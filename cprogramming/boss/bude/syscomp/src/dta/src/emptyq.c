//-------------------------------------------------------------------
// Name      : emptyq.c
// Version   : 0.2.4
// Create    : 2000.6.20
// Update    : 2000.9.19
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : Program used to empty queue.
//-------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "dta.h"

#define USAGE "\nUSAGE: %s queuename \n\n"

int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  int ret;

  if(argc!=2)
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
    printf("DTA_OPENQ(): %s\n",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_EMPTYQ(hqueue))
  {
    printf("DTA_EMPTYQ(): %s\n",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_CLOSEQ(hqueue))
  {
    printf("DTA_CLOSEQ(): %s\n",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_EXIT())
  {
    printf("DTA_EXIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}



