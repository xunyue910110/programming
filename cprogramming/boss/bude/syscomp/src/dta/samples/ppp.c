#include <stdio.h>
#include <unistd.h>
#include "dta.h"

#define MSGLEN 80

int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  int ret;
  FILE *fp;
  char buf[MSGLEN];
  int msglen;
  long i=0;

  if(ret=DTA_INIT())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_OPENQ(&hqueue,argv[2]))
  {
    DTA_CHKERR(ret);
    printf("Open queue failed.\n");
    return -1;
  }
  fp=fopen(argv[1],"r");
  while(!feof(fp))
  {
    msglen=fread(buf,1,MSGLEN,fp);
  read:
    if(ret=DTA_PUTMSG(hqueue,buf,msglen,0))
    {
      if(ret==DTARC_QUEUE_FULL)
      {
        sleep(10);
        goto read;
      }
      DTA_CLOSEQ(hqueue);
      DTA_CHKERR(ret);
      printf("Put message failed.\n");
      return -1;
    }
    i++;
  }
  fclose(fp);
  if(ret=DTA_CLOSEQ(hqueue))
  {
    DTA_CHKERR(ret);
    printf("Close queue failed\n");
    return -1;
  }
  if(ret=DTA_EXIT())
  {
    printf("DTA_EXIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}

