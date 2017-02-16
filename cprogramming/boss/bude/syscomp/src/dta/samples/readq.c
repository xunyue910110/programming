//-------------------------------------------------------------------
// Name      : readq.c
// Version   : 0.2.4
// Update    : 2000.9.19
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : A example program to read message from queue.
//-------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "dta.h"

#define USAGE "\nUSAGE: %s queuename mode <msgs>\n\n"

int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  char *message;
  long msglen;
  long priority;
  long msgs=1;
  long maxmsgs;
  long livmsgs;
  long i=0;
  int ret;

  if(argc<3)
  {
    printf(USAGE,argv[0]);
    return -1;
  }
  if(argc==4)msgs=atol(argv[3]);

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
  if(ret=DTA_ASKQ(hqueue,&maxmsgs,&livmsgs))
  {
    printf("DTA_ASKQ(): %s",DTA_CHKERR(ret));
    DTA_CLOSEQ(hqueue);
    return -1;
  }
  printf("maxmsgs:%ld  livmsgs:%ld\n",maxmsgs,livmsgs);

  while((ret=DTA_GETMSG(&hqueue,NULL,&msglen,&priority,
                        (atoi(argv[2])&DTAGMO_READBYPRIORITY)|DTAGMO_JUSTINQUIRE))==0)
  {
    message=(char *)malloc(msglen);
    DTA_GETMSG(&hqueue,message,&msglen,&priority,atoi(argv[2]));
    printf("%ld <%s>  LEN:%ld  PRI:%ld\n",++i,message,msglen,priority);
    free(message);
    if(--msgs==0)break;
  }
  if(ret=DTA_CLOSEQ(hqueue))
  {
    printf("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_EXIT())
  {
    printf("DTA_EXIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}



