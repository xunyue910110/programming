//-------------------------------------------------------------------
// Name      : createq.c
// Version   : 0.2.4
// Create    : 2000.6.1
// Update    : 2000.9.19
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : Program used to create queue.
//-------------------------------------------------------------------
#include "dtacommon.h"
#include "dta.h"

#define USAGE  "\nUSAGE: %s [-r|-s] [QUEUENAME] <REMOTEHOST> <REMOTEQUEUE> <MAXMSGS>\n\n"

int main(int argc,char *argv[])
{
  char opt;
  int opts;
  DTAHQUEUE hq;
  char dtapath[100];
  char queuename[50];
  char remotehost[50];
  char remotequeue[50];
  int queuetype;
  char fulllistname[100];
  char fullfilename[100];
  FILE *hqlist;
  FILE *hqueue;
  QueueHead queuehead;
  QueueTable *queuetable;
  QUEUELIST queuelist[MAXQUEUES];
  long i;
  int ret;

  opt=getopt(argc,argv,"rs");
  if(opt=='r')
  {
    if(argc<3||argc>4)
    {
      printf(USAGE,argv[0]);
      return -1;
    }
    strcpy(queuename,argv[2]);
    strcpy(remotehost,"NULL");
    strcpy(remotequeue,"NULL");
    if(argc==4)
    {
      queuehead.maxmsgs=atol(argv[3]);
    }
    else
    {
      queuehead.maxmsgs=MAXMSGS;
    }
    queuetype=DTA_QTYPE_RECEIVE;
  }
  else if(opt=='s')
  {
    if(argc<5||argc>6)
    {
      printf(USAGE,argv[0]);
      return -1;
    }
    strcpy(queuename,argv[2]);
    strcpy(remotehost,argv[3]);
    strcpy(remotequeue,argv[4]);
    if(argc==6)
    {
      queuehead.maxmsgs=atol(argv[5]);
    }
    else
    {
      queuehead.maxmsgs=MAXMSGS;
    }
    queuetype=DTA_QTYPE_SEND;
  }
  else
  {
    printf(USAGE,argv[0]);
    return -1;
  }
  queuehead.livmsgs=0;
  queuehead.firstmsgn=1;
  queuehead.lastmsgn=0;
  if(getenv("DTAPATH")==NULL)
  {
    printf("env DTAPATH not set.\n");
    return -1;
  }
  strcpy(dtapath,getenv("DTAPATH"));

  sprintf(fulllistname,"%s/etc/queues.lst",dtapath);

  if((hqlist=fopen(fulllistname,"r"))==NULL)
  {
    printf("First open %s.\n",fulllistname);
  }
  else
  {
    for(i=0;i<MAXQUEUES;i++)
    {
      if(fscanf(hqlist,"%s%d%s%s",queuelist[i].queuename,&queuelist[i].queuetype,
                queuelist[i].remotehost,queuelist[i].remotequeue)<=0)break;
      if(strcmp(queuename,queuelist[i].queuename)==0)
      {
        printf("Queue %s already exist.\n",queuename);
        return -1;
      }
    }
    fclose(hqlist);
    if(i==MAXQUEUES)
    {
      printf("Too much queues, could not append any queue.\n");
      return -1;
    }
  }

  umask(0111);
  if((hqlist=fopen(fulllistname,"a"))==NULL)
  {
    printf("%s could not open.\n",fulllistname);
    return -1;
  }
  sprintf(fullfilename,"%s/queues/%s.q",dtapath,queuename);
  umask(0111);
  if((hqueue=fopen(fullfilename,"wb"))==NULL)
  {
    printf("%s could not create.\n",fullfilename);
    return -1;
  }
  if((queuetable=(QueueTable *)malloc((queuehead.maxmsgs+1)*sizeof(QueueTable)))==NULL)
  {
    printf("Function malloc() failed.\n");
    return -1;
  }
  if(fwrite(&queuehead,sizeof(QueueHead),1,hqueue)!=1)
  {
    printf("fwrite() failed.\n");
    return -1;
  }
  queuetable[0].flag=0;
  queuetable[0].offset=sizeof(QueueHead)+sizeof(QueueTable)*(queuehead.maxmsgs+1);
  queuetable[0].msglen=0;
  queuetable[0].priority=0;
  queuetable[0].foreprioidx=0;
  queuetable[0].nextprioidx=0;
  if(fwrite(&queuetable[0],sizeof(QueueTable),1,hqueue)!=1)
  {
    printf("fwrite() failed.\n");
    return -1;
  }

  for(i=1;i<=queuehead.maxmsgs;i++)
  {
    queuetable[i].flag=0;
    queuetable[i].offset=0;
    queuetable[i].msglen=0;
    queuetable[i].priority=0;
    queuetable[i].foreprioidx=0;
    queuetable[i].nextprioidx=0;
    if(fwrite(&queuetable[i],sizeof(QueueTable),1,hqueue)!=1)
    {
      printf("fwrite() failed.\n");
      return -1;
    }
  }
  fprintf(hqlist,"%s\t%d\t%s\t%s\n",queuename,queuetype,remotehost,remotequeue);
  fclose(hqueue);
  fclose(hqlist);
  free(queuetable);
  if(ret=DTA_INIT())
  {
    printf("DTA_INIT(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(ret=DTA_OPENQ(&hq,queuename))
  {
    printf("DTA_OPENQ(): %s",DTA_CHKERR(ret));
    return -1;
  }
  if(DTA_CLOSEQ(hq))
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


