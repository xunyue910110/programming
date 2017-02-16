//-------------------------------------------------------------------
// Name      : dtaqm.c
// Version   : 0.2.4
// Create    : 2000.6.12
// Update    : 2000.9.19
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : DTAQM is a program to manage queues.
//-------------------------------------------------------------------
#include "dtacommon.h"
#include "dta.h"

QUEUELIST queuelist[MAXQUEUES];
typedef struct
{
  char queuename[100];
  char remotehost[100];
  char remotequeue[100];
}SENDQ_A;
SENDQ_A sendq_a[MAXQUEUES];

void *cleanq(void *a);
void *sendq(void *a);

int main(int argc,char *argv[])
{
  DTAHQUEUE hqueue;
  int ret;
  char fulllistname[100];
  FILE *hqlist;
  pthread_t pd[MAXQUEUES];
  time_t lastsendtime=0;
  time_t lastcleantime=0;
  time_t nowtime;
  long queues;
  long i;
  FILE *fpipe;
  char str[200];
  int num=0;

  signal(SIGCLD,SIG_IGN);
  sprintf(str,"ps -ef | grep ' %s' | grep -v grep",argv[0]);
  if((fpipe=popen(str,"r"))==NULL)
  {
    perr_t("popen() failed!");
    return -1;
  }
  fgets(str,80,fpipe);
  if(fgets(str,80,fpipe)!=NULL)
  {
    perr_t("%s is already startup!",argv[0]);
    pclose(fpipe);
    return -1;
  }
  pclose(fpipe);

  if(ret=DTA_INIT())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  plog_t("%s startup now!",argv[0]);

  sprintf(fulllistname,"%s/etc/queues.lst",dtaenv->dtapath);

  while(1)
  {
    time(&nowtime);
    if((difftime(nowtime,lastsendtime)>QSEND_INTERVAL)||
       (difftime(nowtime,lastcleantime)>QCLEAN_INTERVAL))
    {
      if((hqlist=fopen(fulllistname,"r"))==NULL)
      {
        perr_t("File %s could not open reason: %s",fulllistname,strerror(errno));
        return -1;
      }
      for(i=0;i<MAXQUEUES;i++)
      {
        if(fscanf(hqlist,"%s%d%s%s",queuelist[i].queuename,&queuelist[i].queuetype,
                  queuelist[i].remotehost,queuelist[i].remotequeue)<=0)break;
        //if(ret=DTA_OPENQ(&hqueue,queuelist[i].queuename))
        //  perr_t("DTA_OPENQ(): %s",DTA_CHKERR(ret));
        //if(ret=DTA_CLOSEQ(hqueue))
        //  perr_t("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
      }
      queues=i;
      fclose(hqlist);
    }
    if(difftime(nowtime,lastcleantime)>QCLEAN_INTERVAL)
    {
      for(i=0;i<queues;i++)
      {
        //if(pthread_create(&pd[i],NULL,cleanq,queuelist[i].queuename)!=0)
        //  perr_t("pthread_create(): %s",strerror(errno));
        switch(fork())
        {
        case  0:cleanq(queuelist[i].queuename);
          exit(0);
        case -1:perr_t("fork(): %s",strerror(errno));
          exit(-1);
        }
      }
      lastcleantime=nowtime;
    }
    if(difftime(nowtime,lastsendtime)>QSEND_INTERVAL)
    {
      for(i=0;i<queues;i++)
      {
        if(queuelist[i].queuetype==DTA_QTYPE_SEND)
        {
          strcpy(sendq_a[i].queuename,queuelist[i].queuename);
          strcpy(sendq_a[i].remotehost,queuelist[i].remotehost);
          strcpy(sendq_a[i].remotequeue,queuelist[i].remotequeue);
          //if(pthread_create(&pd[i],NULL,sendq,&sendq_a[i])!=0)
          //  perr_t("pthread_create(): %s",strerror(errno));
          switch(fork())
          {
          case  0:sendq(&sendq_a[i]);
            exit(0);
          case -1:perr_t("fork(): %s",strerror(errno));
            exit(-1);
          }
        }
      }
      lastsendtime=nowtime;
    }
    sleep(1);
  }
  if(ret=DTA_EXIT())
  {
    printf("DTA_EXIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  return 0;
}


void *cleanq(void *a)
{
  DTAHQUEUE hqueue;
  int ret;

  //printf("cleanq:%s\n",(char *)a);
  //  pthread_detach(pthread_self());
  if(ret=DTA_OPENQ(&hqueue,(char *)a))
  {
    perr_t("DTA_OPENQ(): %s",DTA_CHKERR(ret));
    return NULL;
  }
  if(setmlock(hqueue.semid)==0)
  {
    if(ret=DTA_CLEANQ(hqueue))
    {
      perr_t("DTA_CLEANQ(): %s",DTA_CHKERR(ret));
    }
    freemlock(hqueue.semid);
  }
  //  else
  //printf("cleanq:%s locked\n",(char *)a);
  if(ret=DTA_CLOSEQ(hqueue))
  {
    perr_t("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return NULL;
  }
  return NULL;
}


void *sendq(void *a)
{
  DTAHQUEUE hqueue;
  DTAHCONN hconn;
  QueueHead queuehead;
  QueueTable *queuetable;
  char *message;
  long msglen;
  long priority;
  long i;
  int ret;

  //printf("%dsendq:%s\n",pthread_self(),((SENDQ_A *)a)->queuename);
  //  pthread_detach(pthread_self());
  if(ret=DTA_OPENQ(&hqueue,((SENDQ_A *)a)->queuename))
  {
    perr_t("DTA_OPENQ(): %s",DTA_CHKERR(ret));
    return NULL;
  }
  if(setmlock(hqueue.semid)==0)
  {
    if(DTA_GETMSG(&hqueue,NULL,&msglen,&priority,
                  DTAGMO_READBYPRIORITY|DTAGMO_JUSTINQUIRE)==0)
    {
      if(ret=DTA_CONN(&hconn,((SENDQ_A *)a)->remotehost))
      {
        perr_t("DTA_CONN(): %s",DTA_CHKERR(ret));
        DTA_CLOSEQ(hqueue);
        freemlock(hqueue.semid);
        return NULL;
      }
      while(DTA_GETMSG(&hqueue,NULL,&msglen,&priority,
                       DTAGMO_READBYPRIORITY|DTAGMO_JUSTINQUIRE)==0)
      {
        if((message=(char *)malloc(msglen))==NULL)
        {
          perr_t("malloc(): %s",strerror(errno));
          DTA_DISC(hconn);
          DTA_CLOSEQ(hqueue);
          freemlock(hqueue.semid);
          return NULL;
        }
        if(DTA_GETMSG(&hqueue,message,&msglen,&priority,
                      DTAGMO_READBYPRIORITY))break;
        if(ret=DTA_SENDMSG(hconn,((SENDQ_A *)a)->remotequeue,
                           message,msglen,priority))
        {
          perr_t("DTA_SENDMSG(): %s",DTA_CHKERR(ret));
          DTA_DISC(hconn);
          DTA_CLOSEQ(hqueue);
          freemlock(hqueue.semid);
          free(message);
          return NULL;
        }
        if(ret=DTA_GETMSG(&hqueue,message,&msglen,&priority,
                          DTAGMO_READBYPRIORITY|DTAGMO_DELETEAFTERREAD))
        {
          perr_t("DTA_GETMSG(): %s",DTA_CHKERR(ret));
          DTA_DISC(hconn);
          DTA_CLOSEQ(hqueue);
          freemlock(hqueue.semid);
          free(message);
          return NULL;
        }
        free(message);
      }
      if(ret=DTA_DISC(hconn))
      {
        perr_t("DTA_DISC(): %s",DTA_CHKERR(ret));
        DTA_CLOSEQ(hqueue);
        freemlock(hqueue.semid);
        return NULL;
      }
    }
    freemlock(hqueue.semid);
  }
  //  else 
  //printf("sendq:%s locked\n",((SENDQ_A *)a)->queuename);
  if(DTA_CLOSEQ(hqueue))
  {
    perr_t("DTA_CLOSEQ(): %s",DTA_CHKERR(ret));
    return NULL;
  }
  return NULL;
}

