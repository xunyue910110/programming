//-------------------------------------------------------------------
// Name      : deleteq.c
// Version   : 0.2.4
// Create    : 2000.6.1
// Update    : 2000.9.19
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : Program used to delete queue.
//-------------------------------------------------------------------
#include "dtacommon.h"
#include "dta.h"

#define USAGE  "\nUSAGE: %s [QUEUENAME]\n\n"

int main(int argc,char *argv[])
{
  char dtapath[100];
  char queuename[50];
  int queuetype;
  char fulllistname[100];
  char templistname[100];
  char fullfilename[100];
  FILE *hqlist;
  FILE *hqueue;
  FILE *htmp;
  QueueHead queuehead;
  QUEUELIST queuelist[MAXQUEUES];
  long i;
  char answer[10];
  char syscmd[100];
  int semid;

  if(argc!=2)
  {
    printf(USAGE,argv[0]);
    return -1;
  }
  if(getenv("DTAPATH")==NULL)
  {
    printf("env DTAPATH not set.\n");
    return -1;
  }
  strcpy(dtapath,getenv("DTAPATH"));
  strcpy(queuename,argv[1]);
  sprintf(fullfilename,"%s/queues/%s.q",dtapath,queuename);
  if((hqueue=fopen(fullfilename,"rb"))==NULL)
  {
    printf("%s could not open.\n",fullfilename);
    return -1;
  }
  if(fseek(hqueue,0,SEEK_SET))
  {
    return DTARC_FILESEEK_ERROR;
  }
  if(fread(&queuehead,1,sizeof(QueueHead),hqueue)!=sizeof(QueueHead))
  {
    return DTARC_FILEREAD_ERROR;
  }
  if(queuehead.livmsgs>0)
  {
    printf("This queue is not empty. are you sure to delete? [y/n]");
    scanf("%s",answer);
    if(toupper(answer[0])!='Y')
    {
      printf("This queue is not deleted.\n");
      return -1;
    }
  }
  fclose(hqueue);
  if((semid=semget(ftok(fullfilename,1),0,0666))>=0)
  {
    //RETURN_ON_FAIL(DTARC_SEMOPEN_ERROR);
    if(semctl(semid,0,IPC_RMID)<0)
    {
      printf("delete semid error\n");
    }
  }
  sprintf(syscmd,"rm %s",fullfilename);
  system(syscmd);

  sprintf(fulllistname,"%s/etc/queues.lst",dtapath);
  sprintf(templistname,"%s/etc/queues.lst.tmp",dtapath);
  if((hqlist=fopen(fulllistname,"r"))==NULL)
  {
    printf("%s could not open.\n",fulllistname);
    return -1;
  }
  umask(0111);
  if((htmp=fopen(templistname,"w"))==NULL)
  {
    printf("%s could not open.\n",templistname);
    return -1;
  }
  for(i=0;i<MAXQUEUES;i++)
  {
    if(fscanf(hqlist,"%s%d%s%s",queuelist[i].queuename,&queuelist[i].queuetype,
              queuelist[i].remotehost,queuelist[i].remotequeue)<=0)break;
    if(strcmp(queuename,queuelist[i].queuename))
    {
      fprintf(htmp,"%s\t%d\t%s\t%s\n",queuelist[i].queuename,
              queuelist[i].queuetype,queuelist[i].remotehost,
              queuelist[i].remotequeue);
    }
  }
  if(fclose(htmp))
  {
    printf("fclose() failed\n");
    return -1;
  }
  if(fclose(hqlist))
  {
    printf("fclose() failed\n");
    return -1;
  }
  sprintf(syscmd,"mv %s %s",templistname,fulllistname);
  system(syscmd);
  return 0;
}


