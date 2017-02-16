#include "dtacommon.h"
#include "dta.h"
#include <sys/time.h>
char dtaef[20];
int dtael;
DTAENV *dtaenv;

struct timeval time0;
struct timeval time1;
struct timezone tzp;

int sendmsg_t(DTAHCONN hconn,const char *remotequeue,
              const char *message,long msglen,long priority);
int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  DTAHCONN hconn;
  QueueHead queuehead;
  QueueTable *queuetable;
  char *message;
  long msglen;
  long priority;
  int ret;
  char fulllistname[100];
  time_t lastsendtime=0;
  time_t lastcleantime=0;
  time_t nowtime;
  long queues;
  long i;
  char str[200];
  int num=0;

  if(ret=DTA_INIT())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_OPENQ(&hqueue,argv[1]))
  {
    perr_t("DTA_OPENQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(setmlock(hqueue.semid)==0)
  {
    if(DTA_GETMSG(&hqueue,NULL,&msglen,&priority,
                  DTAGMO_READBYPRIORITY|DTAGMO_JUSTINQUIRE)==0)
    {
      if(ret=DTA_CONN(&hconn,argv[2]))
      {
        perr_t("DTA_CONN(): %s",DTA_CHKERR(ret));
        DTA_CLOSEQ(hqueue);
        freemlock(hqueue.semid);
        return -1;
      }
      int on=1;
      setsockopt(hconn.fd,SOL_SOCKET,SO_OOBINLINE,&on,sizeof(on));

      while(DTA_GETMSG(&hqueue,NULL,&msglen,&priority,
                       DTAGMO_READBYPRIORITY|DTAGMO_JUSTINQUIRE)==0)
      {
        gettimeofday(&time0,&tzp);
        printf("%ld\n",time0.tv_sec*1000000+time0.tv_usec);
        if((message=(char *)malloc(msglen))==NULL)
        {
          perr_t("malloc(): %s",strerror(errno));
          DTA_DISC(hconn);
          DTA_CLOSEQ(hqueue);
          freemlock(hqueue.semid);
          return -1;
        }
        if(DTA_GETMSG(&hqueue,message,&msglen,&priority,
                      DTAGMO_READBYPRIORITY))break;
        gettimeofday(&time0,&tzp);
        printf("startsend:%ld\n",time0.tv_sec*1000000+time0.tv_usec);
        if(ret=sendmsg_t(hconn,argv[3],
                         message,msglen,priority))
        {
          perr_t("DTA_SENDMSG(): %s",DTA_CHKERR(ret));
          DTA_DISC(hconn);
          DTA_CLOSEQ(hqueue);
          freemlock(hqueue.semid);
          free(message);
          return -1;
        }
        gettimeofday(&time0,&tzp);
        printf("endsend:%ld\n",time0.tv_sec*1000000+time0.tv_usec);
        if(ret=DTA_GETMSG(&hqueue,message,&msglen,&priority,
                          DTAGMO_READBYPRIORITY|DTAGMO_DELETEAFTERREAD))
        {
          perr_t("DTA_GETMSG(): %s",DTA_CHKERR(ret));
          DTA_DISC(hconn);
          DTA_CLOSEQ(hqueue);
          freemlock(hqueue.semid);
          free(message);
          return -1;
        }
        free(message);
        gettimeofday(&time0,&tzp);
        printf("%ld\n",time0.tv_sec*1000000+time0.tv_usec);
      }
      if(ret=DTA_DISC(hconn))
      {
        perr_t("DTA_DISC(): %s",DTA_CHKERR(ret));
        DTA_CLOSEQ(hqueue);
        freemlock(hqueue.semid);
        return -1;
      }
    }
    freemlock(hqueue.semid);
  }
  if(DTA_CLOSEQ(hqueue))
  {
    perr_t("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_EXIT())
  {
    printf("DTA_EXIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}


int sendmsg_t(DTAHCONN hconn,const char *remotequeue,
    const char *message,long msglen,long priority)
{
  char retc[1001];
  char smsgbuf[SMSGLEN];

  memset(smsgbuf,0,SMSGLEN);
  strcpy(smsgbuf,"sendmsg");
  if(write(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }

  gettimeofday(&time0,&tzp);
  printf("write1:%ld\n",time0.tv_sec*1000000+time0.tv_usec);

  //read(hconn.fd,smsgbuf,1);
  //gettimeofday(&time0,&tzp);
  //printf("read1:%ld\n",time0.tv_sec*1000000+time0.tv_usec);

  memset(smsgbuf,0,SMSGLEN);
  strcpy(smsgbuf,remotequeue);
  if(write(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  gettimeofday(&time0,&tzp);
  printf("write2:%ld\n",time0.tv_sec*1000000+time0.tv_usec);
  //read(hconn.fd,smsgbuf,1);
  //gettimeofday(&time0,&tzp);
  //printf("read1:%ld\n",time0.tv_sec*1000000+time0.tv_usec);

  memset(smsgbuf,0,SMSGLEN);
  sprintf(smsgbuf,"%10ld %10ld",priority,msglen);
  if(write(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  gettimeofday(&time0,&tzp);
  printf("write3:%ld\n",time0.tv_sec*1000000+time0.tv_usec);
  //read(hconn.fd,smsgbuf,1);
  //gettimeofday(&time0,&tzp);
  //printf("read1:%ld\n",time0.tv_sec*1000000+time0.tv_usec);
  if(write(hconn.fd,message,msglen)!=msglen)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  gettimeofday(&time0,&tzp);
  printf("write4:%ld\n",time0.tv_sec*1000000+time0.tv_usec);

  //if(recv(hconn.fd,retc,1,MSG_OOB)!=1)
  if(read(hconn.fd,retc,1)!=1)
  {
    RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
  }
  gettimeofday(&time0,&tzp);
  printf("read:%ld\n",time0.tv_sec*1000000+time0.tv_usec);
  if(retc[0]=='0')
  {
    RETURN_ON_FAIL(-1);
  }

  return 0;
}
