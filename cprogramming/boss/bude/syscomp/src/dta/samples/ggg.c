#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dta.h"

int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  FILE *fp;
  char *message;
  long msglen;
  long priority;
  long msgs=1;
  long maxmsgs;
  long livmsgs;
  long i=0;
  int ret;

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
  fp=fopen(argv[1],"w");
  while(1)
  {
    ret=DTA_GETMSG(&hqueue,message,&msglen,&priority,
                   DTAGMO_RESETCURSOR|DTAGMO_READBYORDER|DTAGMO_JUSTINQUIRE);
    if(ret==DTARC_QUEUE_EMPTY)
    {
      fflush(fp);
      break;
      sleep(10);
      continue;
    }
    if(ret!=0)
    {
      DTA_CHKERR(ret);
      break;
    }
    message=(char *)malloc(msglen);
    DTA_GETMSG(&hqueue,message,&msglen,&priority,
               DTAGMO_RESETCURSOR|DTAGMO_READBYPRIORITY|DTAGMO_DELETEAFTERREAD);
    fwrite(message,1,msglen,fp);
    fflush(fp);
    free(message);
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

