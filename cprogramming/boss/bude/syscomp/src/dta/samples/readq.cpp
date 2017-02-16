#include "mt.h"
#include <stdio.h>
#include <stdlib.h>

#define USAGE "\nUSAGE: %s queuename mode <msgs>\n\n"

int main(int argc,char *argv[])
{
  CMsgQueue MQ;
  char *message;
  //char message[1000];
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
  if(ret=MQ.ask(maxmsgs,livmsgs))
  {
    printf("DTA_ASKQ(): %s",DTA_CHKERR(ret));
    MQ.close();
    return -1;
  }
  printf("maxmsgs:%ld  livmsgs:%ld\n",maxmsgs,livmsgs);

  while((ret=MQ.get(NULL,msglen,priority,
                    (atoi(argv[2])&DTAGMO_READBYPRIORITY)|DTAGMO_JUSTINQUIRE))==0)
  {
    message=(char *)malloc(msglen);
    MQ.get(message,msglen,priority,atoi(argv[2]));
    printf("%ld <%s>  LEN:%ld  PRI:%ld\n",++i,message,msglen,priority);
    free(message);
    if(--msgs==0)break;
  }
  if(ret=MQ.close())
  {
    printf("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=MQ.end())
  {
    printf("DTA_EXIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}



