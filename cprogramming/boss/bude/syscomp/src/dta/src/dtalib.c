//-------------------------------------------------------------------
// Name      : dtalib.c
// Version   : 0.2.8
// Create    : 2000.6.5
// Update    : 2002.4.4
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : DTA base function library.
//-------------------------------------------------------------------
#include "dtacommon.h"
#include "dta.h"

char dtaef[20];
int dtael;
DTAENV *dtaenv;
char *resultarray;
int resultlength;
int conntimeo;
int rcvtimeo;
int sndtimeo;

//-------------------------------------------------------------------
int DTA_INIT()
{
  int shmid;

  signal(SIGPIPE,SIG_IGN);
  shmid=shmget(29999,sizeof(DTAENV),0644);
  if(shmid==-1)RETURN_ON_FAIL(DTARC_DTAENVGET_ERROR);
  dtaenv=(DTAENV *)shmat(shmid,0,0);
  conntimeo=30;
  rcvtimeo=30;
  sndtimeo=30;
  return 0;
}


//-------------------------------------------------------------------
int DTA_EXIT()
{
  if(shmdt((char *)dtaenv))RETURN_ON_FAIL(DTARC_DTAENVLOSE_ERROR);
  return 0;
}


//-------------------------------------------------------------------
int DTA_CONN(DTAHCONN *hconn,const char *host)
{
  char service[]="DTAServer";
  char transport[]="tcp";
  struct hostent *phe;
  struct servent *pse;
  struct protoent *ppe;
  struct sockaddr_in sin;
  int s;
  int type;
  Sigfunc *sigfunc;

  memset(&sin,0,sizeof(sin));
  sin.sin_family=AF_INET;
  /*
    if(pse=getservbyname(service,transport))
    {
    sin.sin_port=pse->s_port;
    }
    else if((sin.sin_port=htons((u_short)atoi(service)))==0)
    {
    RETURN_ON_FAIL(DTARC_SERVICEPORT_ERROR);
    }
  */
  sin.sin_port=htons(29999);
  if(phe=gethostbyname(host))
  {
    memcpy(&sin.sin_addr,phe->h_addr,phe->h_length);
  }
  else if((sin.sin_addr.s_addr=inet_addr(host))==INADDR_NONE)
  {
    RETURN_ON_FAIL(DTARC_REMOTEHOSTIP_ERROR);
  }
  if((ppe=getprotobyname(transport))==0)
  {
    RETURN_ON_FAIL(DTARC_PROTOTYPE_ERROR);
  }
  if(strcmp(transport,"udp")==0)
  {
    type=SOCK_DGRAM;
  }
  else
  {
    type=SOCK_STREAM;
  }
  if((s=socket(PF_INET,type,ppe->p_proto))<0)
  {
    RETURN_ON_FAIL(DTARC_SOCKETCREATE_ERROR);
  }
  sigfunc=signal(SIGALRM,dealalarm);
  alarm(conntimeo);
  if(connect(s,(struct sockaddr *)&sin,sizeof(sin))<0)
    //sockaddr->sockaddr_in
  {
  	if(s)
  		close(s);
    alarm(0);
    signal(SIGALRM,sigfunc);
    RETURN_ON_FAIL(DTARC_SOCKETCONNECT_ERROR);
  }
  alarm(0);
  signal(SIGALRM,sigfunc);
  hconn->fd=s;
  return 0;
}


//-------------------------------------------------------------------
int DTA_DISC(DTAHCONN hconn)
{
  if(close(hconn.fd))
  {
    RETURN_ON_FAIL(DTARC_SOCKETCLOSE_ERROR);
  }
  return 0;
}


//-------------------------------------------------------------------
int DTA_PUT(DTAHCONN hconn,const char *sourcefile,
            const char *targetfile,DTAFTINFO *ftinfo)
{
  FILE *hfile;
  char buf[BUFFERLENGTH];
  int readsize;
  int startpos;
  int filelength;
  struct stat filestat;
  char smsgbuf[SMSGLEN];
  int longvalue;
  time_t lastmoditime;

  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,"putfile",sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,sourcefile,sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,targetfile,sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  stat(sourcefile,&filestat);
  filelength=filestat.st_size;
  lastmoditime=filestat.st_mtime;
  longvalue=htonl(filelength);
  if(writen(hconn.fd,&longvalue,sizeof(int))!=sizeof(int))
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  longvalue=htonl(lastmoditime);
  if(writen(hconn.fd,&longvalue,sizeof(time_t))!=sizeof(time_t))
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  if(readn(hconn.fd,&longvalue,sizeof(int))!=sizeof(int))
  {
    RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
  }
  startpos=ntohl(longvalue);
  if((hfile=fopen(sourcefile,"rb"))==NULL)
  {
    RETURN_ON_FAIL(DTARC_FILEOPEN_ERROR);
  }
  if(fseek(hfile,startpos,SEEK_SET))
  {
    fclose(hfile);
    RETURN_ON_FAIL(DTARC_FILESEEK_ERROR);
  }
  while(!feof(hfile))
  {
    readsize=fread(buf,1,BUFFERLENGTH,hfile);
    if(readsize==0)break;
    if(writen(hconn.fd,buf,readsize)!=readsize)
    {
      fclose(hfile);
      RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
    }
  }
  if(fclose(hfile))
  {
    RETURN_ON_FAIL(DTARC_FILECLOSE_ERROR);
  }
  if(readn(hconn.fd,smsgbuf,1)!=1)
  {
    RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
  }
  if(smsgbuf[0]=='0')
  {
    RETURN_ON_FAIL(-1);
  }
  ftinfo->filesize=filelength;
  ftinfo->startpos=startpos;
  return 0;
}


//-------------------------------------------------------------------
int DTA_GET(DTAHCONN hconn,const char *sourcefile,
            const char *targetfile,DTAFTINFO *ftinfo)
{
  FILE *hfile;
  char buf[BUFFERLENGTH];
  int readsize;
  char tmpfile[200];
  int filelength;
  struct stat filestat;
  char syscmd[1000];
  int startpos;
  int readtotal;
  int longvalue;
  char smsgbuf[SMSGLEN];

  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,"getfile",sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,sourcefile,sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  if(strrchr(targetfile,'/')==NULL)
  {
    sprintf(tmpfile,"%s/tmp/%s",dtaenv->dtapath,targetfile);
  }
  else
  {
    sprintf(tmpfile,"%s",targetfile);
  }
  if(stat(tmpfile,&filestat))
  {
    startpos=0;
  }
  else
  {
    startpos=filestat.st_size;
  }
  printf("write longvalue=%ld,%ld\n",startpos,longvalue);
  longvalue=htonl(startpos);
  if(writen(hconn.fd,&longvalue,sizeof(int))!=sizeof(int))
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  
  if(readn(hconn.fd,&longvalue,sizeof(int))!=sizeof(int))
  {
    RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
  }
  printf("read longvalue=%ld\n",longvalue);
  filelength=ntohl(longvalue);
  if(filelength<0)
  {
    RETURN_ON_FAIL(DTARC_FILEOPEN_ERROR);
  }
  if((hfile=fopen(tmpfile,"a+b"))==NULL)
  {
    RETURN_ON_FAIL(DTARC_FILEOPEN_ERROR);
  }
  readtotal=startpos;
  while(readtotal<filelength)
  {
    
    printf("readtotal=%ld,filelength=%ld\n",readtotal,filelength);

    readsize=readn(hconn.fd,buf,sizeof(buf)>filelength-readtotal?
                   filelength-readtotal:sizeof(buf));
    if(readsize<=0)
    {
      printf("error here! read len=%d\n",sizeof(buf)>filelength-readtotal?
                   filelength-readtotal:sizeof(buf));
      printf("sock=%d, filelength=%d, readtotal=%d\n",hconn.fd,filelength,readtotal);
      fclose(hfile);
      RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
    }
    if(fwrite(buf,1,readsize,hfile)!=readsize)
    {
      fclose(hfile);
      RETURN_ON_FAIL(DTARC_FILEWRITE_ERROR);
    }
    if(fflush(hfile))
    {
      fclose(hfile);
      RETURN_ON_FAIL(DTARC_FILEFLUSH_ERROR);
    }
    readtotal+=readsize;
  }
  if(fclose(hfile))
  {
    RETURN_ON_FAIL(DTARC_FILECLOSE_ERROR);
  }
  ftinfo->filesize=filelength;
  ftinfo->startpos=startpos;
  return 0;
}


//-------------------------------------------------------------------
int DTA_RESULT(char *array,int length)
{
  resultarray=array;
  resultlength=length;
  return 0;
}


//-------------------------------------------------------------------
int DTA_RCMD(DTAHCONN hconn,const char *command,
             int result,int *resultrow)
{
  char smsgbuf[SMSGLEN];
  int longvalue;
  int rret;
  int i;

  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,"execrcmd",sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,command,sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  longvalue=htonl(result);
  if(writen(hconn.fd,&longvalue,sizeof(int))!=sizeof(int))
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  if(readn(hconn.fd,&longvalue,sizeof(int))!=sizeof(int))
  {
    RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
  }
  rret=ntohl(longvalue);
  i=0;
  do
  {
    memset(smsgbuf,0,SMSGLEN);
    if(readn(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
    {
      RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
    }
    strncpy((char *)(resultarray+i*resultlength),
            smsgbuf,resultlength);
    memset(resultarray+i*resultlength+resultlength-1,0,1);
    i++;
  }while(strcmp(smsgbuf,"DTA:end of result."));
  *resultrow=i-1;
  //if(rret)RETURN_ON_FAIL(DTARC_EXECRCMD_ERROR);
  //return 0;
  return rret;
}


//-------------------------------------------------------------------
int DTA_OPENQ(DTAHQUEUE *hqueue,const char *queuename)
{
  char fullfilename[100];
  union semun argr;
  union semun argw;
  union semun argm;
  QueueHead queuehead;

  sprintf(fullfilename,"%s/queues/%s.q",dtaenv->dtapath,queuename);
  if((hqueue->fp=fopen(fullfilename,"r+b"))==NULL)
  {
    RETURN_ON_FAIL(DTARC_FILEOPEN_ERROR);
  }
  strncpy(hqueue->queuename,queuename,sizeof(hqueue->queuename));
  strncpy(hqueue->fullfilename,fullfilename,
          sizeof(hqueue->fullfilename));
  hqueue->cursor=0;
  if((hqueue->semid=semget(ftok(fullfilename,1),3,
                           IPC_CREAT|IPC_EXCL|0666))>=0)
  {
    argr.val=0;
    argw.val=0;
    argm.val=0;
    semctl(hqueue->semid,0,SETVAL,argr);
    semctl(hqueue->semid,1,SETVAL,argw);
    semctl(hqueue->semid,2,SETVAL,argm);
  }
  else if(errno==EEXIST)
  {
    if((hqueue->semid=semget(ftok(fullfilename,1),0,0666))<0)
    {
      fclose(hqueue->fp);
      RETURN_ON_FAIL(DTARC_SEMOPEN_ERROR);
    }
  }
  else
  {
    fclose(hqueue->fp);
    RETURN_ON_FAIL(DTARC_SEMCREATE_ERROR);
  }

  if(fseek(hqueue->fp,0,SEEK_SET))
  {
    RETURN_ON_FAIL(DTARC_FILESEEK_ERROR);
  }
  if(fread(&queuehead,1,sizeof(QueueHead),hqueue->fp)!=
     sizeof(QueueHead))
  {
    RETURN_ON_FAIL(DTARC_FILEREAD_ERROR);
  }
  hqueue->tablelen=sizeof(QueueHead)+(queuehead.maxmsgs+1)*
    sizeof(QueueTable);
  if((hqueue->tablemap=mmap(NULL,hqueue->tablelen,PROT_READ|PROT_WRITE,
                            MAP_SHARED,fileno(hqueue->fp),0))==MAP_FAILED)
  {
    RETURN_ON_FAIL(DTARC_MEMORYMAP_ERROR);
  }
  return 0;
}


//-------------------------------------------------------------------
int DTA_CLOSEQ(DTAHQUEUE hqueue)
{
  if(munmap((char *)hqueue.tablemap,hqueue.tablelen))
  {
    RETURN_ON_FAIL(DTARC_MEMORYUNMAP_ERROR);
  }
  if(fclose(hqueue.fp))
  {
    RETURN_ON_FAIL(DTARC_FILECLOSE_ERROR);
  }
  return 0;
}


//-------------------------------------------------------------------
int DTA_ASKQ(DTAHQUEUE hqueue,long *maxmsgs,long *livmsgs)
{
  QueueHead *queuehead;
  long i;
  long j=0;

  queuehead=(QueueHead *)hqueue.tablemap;
  *maxmsgs=queuehead->maxmsgs;
  *livmsgs=queuehead->livmsgs;
  return 0;
}


//-------------------------------------------------------------------
int DTA_EMPTYQ(DTAHQUEUE hqueue)
{
  QueueHead *queuehead;
  QueueTable *queuetable;
  long i;

  setwlock(hqueue.semid);
  queuehead=(QueueHead *)hqueue.tablemap;
  queuetable=(QueueTable *)((char *)hqueue.tablemap+sizeof(QueueHead));
  queuehead->livmsgs=0;
  queuehead->firstmsgn=1;
  queuehead->lastmsgn=0;

  queuetable[0].flag=0;
  queuetable[0].offset=sizeof(QueueHead)+
    sizeof(QueueTable)*(queuehead->maxmsgs+1);
  queuetable[0].msglen=0;
  queuetable[0].priority=0;
  queuetable[0].foreprioidx=0;
  queuetable[0].nextprioidx=0;

  for(i=1;i<=queuehead->maxmsgs;i++)
  {
    queuetable[i].flag=0;
    queuetable[i].offset=0;
    queuetable[i].msglen=0;
    queuetable[i].priority=0;
    queuetable[i].foreprioidx=0;
    queuetable[i].nextprioidx=0;
  }
  /*
    if(msync(hqueue.tablemap,hqueue.tablelen,MS_SYNC))
    {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_FILEWRITE_ERROR);
    }
  */
  if(truncate(hqueue.fullfilename,queuetable[0].offset))
  {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_FILETRUNCATE_ERROR);
  }
  freewlock(hqueue.semid);
  return 0;
}


//-------------------------------------------------------------------
int DTA_CLEANQ(DTAHQUEUE hqueue)
{
  QueueHead *queuehead;
  char message[512];
  long i;
  long j;
  int ret;

  setwlock(hqueue.semid);
  queuehead=(QueueHead *)hqueue.tablemap;
  if(queuehead->livmsgs==0)
  {
    freewlock(hqueue.semid);
    if(ret=DTA_EMPTYQ(hqueue))
    {
      DTA_CHKERR(ret);
      RETURN_ON_FAIL(DTARC_INTERNAL_ERROR);
    }
    return 0;
  }
  freewlock(hqueue.semid);
  return 0;
}


//-------------------------------------------------------------------
int DTA_PUTMSG(DTAHQUEUE hqueue,const char *message,long msglen,
               long priority)
{
  QueueHead *queuehead;
  QueueTable *queuetable;
  long i;
  long currentmsgn;
  struct stat filestat;
  int ret;

  setwlock(hqueue.semid);
  queuehead=(QueueHead *)hqueue.tablemap;
  queuetable=(QueueTable *)((char *)hqueue.tablemap+sizeof(QueueHead));
  if(queuehead->livmsgs==queuehead->maxmsgs)
  {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_QUEUE_FULL);
  }
  queuehead->livmsgs++;
  if(queuehead->lastmsgn==queuehead->maxmsgs)
  {
    for(i=1;i<queuehead->maxmsgs;i++)
    {
      if(queuetable[i].flag==0)break;
    }
    currentmsgn=i;
  }
  else currentmsgn=queuehead->lastmsgn+1;
  queuetable[currentmsgn].flag=1;
  stat(hqueue.fullfilename,&filestat);
  queuetable[currentmsgn].offset=filestat.st_size;

  queuetable[currentmsgn].msglen=msglen;
  queuetable[currentmsgn].priority=priority;

  if(currentmsgn<queuehead->firstmsgn)queuehead->firstmsgn=currentmsgn;
  if(currentmsgn>queuehead->lastmsgn)queuehead->lastmsgn=currentmsgn;

  for(i=queuetable[0].nextprioidx;
      queuetable[currentmsgn].priority<=queuetable[i].priority;)
  {
    i=0;break;//no priority!!!!!
    if((i=queuetable[i].nextprioidx)==0)break;
  }
  queuetable[currentmsgn].foreprioidx=queuetable[i].foreprioidx;
  queuetable[currentmsgn].nextprioidx=i;
  queuetable[queuetable[i].foreprioidx].nextprioidx=currentmsgn;
  queuetable[i].foreprioidx=currentmsgn;

  //if(msync(hqueue.tablemap,hqueue.tablelen,MS_SYNC))
  //{
  //  freewlock(hqueue.semid);
  //  RETURN_ON_FAIL(DTARC_FILEWRITE_ERROR);
  //}
  if(fseek(hqueue.fp,queuetable[currentmsgn].offset,SEEK_SET))
  {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_FILESEEK_ERROR);
  }
  if(fwrite(message,1,msglen,hqueue.fp)!=msglen)
  {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_FILEWRITE_ERROR);
  }
  if(fflush(hqueue.fp))
  {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_FILEFLUSH_ERROR);
  }
  freewlock(hqueue.semid);
  return 0;
}


//-------------------------------------------------------------------
int DTA_GETMSG(DTAHQUEUE *hqueue,char *message,long *msglen,
               long *priority,int mode)
{
  QueueHead *queuehead;
  QueueTable *queuetable;
  long curmsgn;
  int ret;

  setrlock(hqueue->semid);
  queuehead=(QueueHead *)hqueue->tablemap;
  queuetable=(QueueTable *)((char *)hqueue->tablemap+sizeof(QueueHead));
  if(queuehead->livmsgs==0)
  {
    freerlock(hqueue->semid);
    RETURN_ON_FAIL(DTARC_QUEUE_EMPTY);
  }
  if(mode&DTAGMO_RESETCURSOR)hqueue->cursor=0;

  curmsgn=hqueue->cursor;
  if(mode&DTAGMO_READBYPRIORITY)
  {
    if(queuetable[curmsgn].flag==0)
    {
      hqueue->cursor=0;
      curmsgn=hqueue->cursor;
    }
    if((curmsgn=queuetable[curmsgn].nextprioidx)==0)
    {
      freerlock(hqueue->semid);
      RETURN_ON_FAIL(DTARC_QUEUE_END);
    }
  }
  else
  {
    if(curmsgn==0)curmsgn=queuehead->firstmsgn-1;
    while(curmsgn++)
    {
      if(curmsgn>queuehead->lastmsgn)
      {
        freerlock(hqueue->semid);
        RETURN_ON_FAIL(DTARC_QUEUE_END);
      }
      if(queuetable[curmsgn].flag==1)break;
    }
  }
  if(!(mode&DTAGMO_JUSTINQUIRE))
  {
    if(fseek(hqueue->fp,queuetable[curmsgn].offset,SEEK_SET))
    {
      freerlock(hqueue->semid);
      RETURN_ON_FAIL(DTARC_FILESEEK_ERROR);
    }
    if(fread(message,1,queuetable[curmsgn].msglen,hqueue->fp)!=
       queuetable[curmsgn].msglen)
    {
      freerlock(hqueue->semid);
      RETURN_ON_FAIL(DTARC_FILEREAD_ERROR);
    }
  }
  *msglen=queuetable[curmsgn].msglen;
  *priority=queuetable[curmsgn].priority;
  if(mode&DTAGMO_DELETEAFTERREAD)
  {
    freerlock(hqueue->semid);
    if(ret=DTA_DELMSG(*hqueue,curmsgn))
    {
      DTA_CHKERR(ret);
      RETURN_ON_FAIL(DTARC_INTERNAL_ERROR);
    }
    return 0;
  }
  if(mode&DTAGMO_SKIPWHILEREAD)hqueue->cursor=curmsgn;
  freerlock(hqueue->semid);
  return 0;
}


//-------------------------------------------------------------------
int DTA_DELMSG(DTAHQUEUE hqueue,long msgn)
{
  QueueHead *queuehead;
  QueueTable *queuetable;

  setwlock(hqueue.semid);
  queuehead=(QueueHead *)hqueue.tablemap;
  queuetable=(QueueTable *)((char *)hqueue.tablemap+sizeof(QueueHead));
  if(queuetable[msgn].flag==0)
  {
    freewlock(hqueue.semid);
    RETURN_ON_FAIL(DTARC_MESSAGE_DIED);
  }
  queuetable[queuetable[msgn].foreprioidx].nextprioidx=
    queuetable[msgn].nextprioidx;
  queuetable[queuetable[msgn].nextprioidx].foreprioidx=
    queuetable[msgn].foreprioidx;
  queuetable[msgn].flag=0;
  queuehead->livmsgs--;
  if(queuehead->livmsgs>0)
  {
    if(msgn==queuehead->firstmsgn)
      while(queuetable[++queuehead->firstmsgn].flag==0);
    if(msgn==queuehead->lastmsgn)
      while(queuetable[--queuehead->lastmsgn].flag==0);
  }
  else
  {
    queuehead->firstmsgn=1;
    queuehead->lastmsgn=0;
  }
  if(queuehead->firstmsgn>queuehead->lastmsgn)
  {
    queuehead->firstmsgn=1;
    queuehead->lastmsgn=0;
  }
  //if(msync(hqueue.tablemap,hqueue.tablelen,MS_SYNC))
  //{
  //  freewlock(hqueue.semid);
  //  RETURN_ON_FAIL(DTARC_FILEWRITE_ERROR);
  //}
  freewlock(hqueue.semid);
  return 0;
}


//-------------------------------------------------------------------
int DTA_SETOPT(int optionname,const void *optionvalue,
               size_t optionlength)
{
  switch(optionname)
  {
  case DTAOPT_CONNTIMEO:
    conntimeo=*((int *)optionvalue);
    break;
  case DTAOPT_RCVTIMEO:
    rcvtimeo=*((int *)optionvalue);
    break;
  case DTAOPT_SNDTIMEO:
    sndtimeo=*((int *)optionvalue);
    break;
  default:;
  }
  return 0;
}

//-------------------------------------------------------------------
int DTA_SENDMSG(DTAHCONN hconn,const char *remotequeue,
                const char *message,long msglen,long priority)
{
  char retc[SMSGLEN];
  char smsgbuf[SMSGLEN];

  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,"sendmsg",sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  readn(hconn.fd,retc,1);
  memset(smsgbuf,0,SMSGLEN);
  strncpy(smsgbuf,remotequeue,sizeof(smsgbuf));
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  readn(hconn.fd,retc,1);
  memset(smsgbuf,0,SMSGLEN);
  sprintf(smsgbuf,"%10ld %10ld",priority,msglen);
  if(writen(hconn.fd,smsgbuf,SMSGLEN)!=SMSGLEN)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  readn(hconn.fd,retc,1);
  if(writen(hconn.fd,message,msglen)!=msglen)
  {
    RETURN_ON_FAIL(DTARC_SOCKETWRITE_ERROR);
  }
  if(readn(hconn.fd,retc,1)!=1)
  {
    RETURN_ON_FAIL(DTARC_SOCKETREAD_ERROR);
  }
  if(retc[0]=='0')
  {
    RETURN_ON_FAIL(-1);
  }
  return 0;
}


//-------------------------------------------------------------------
char *DTA_CHKERR(int err)
{
  static char errmsg[200];

  if(err==0)return NULL;
  sprintf(errmsg,"DTAERROR: %d LOC(%s:%d) REASON: ",err,dtaef,dtael);

  switch(err)
  {
  case DTARC_OK:
    break;
  case DTARC_QUEUE_EMPTY:
    strcat(errmsg,"Queue is empty.");
    break;
  case DTARC_QUEUE_FULL:
    strcat(errmsg,"Queue is full.");
    break;
  case DTARC_QUEUE_END:
    strcat(errmsg,"Queue cursor touch end.");
    break;
  case DTARC_MESSAGE_DIED:
    strcat(errmsg,"Message already died.");
    break;
  case DTARC_GENERAL_ERROR:
    strcat(errmsg,"General error.");
    break;
  case DTARC_INTERNAL_ERROR:
    strcat(errmsg,"Internal error.");
    break;
  case DTARC_DTAENV_ERROR:
    strcat(errmsg,"DTAENV not set.");
    break;
  case DTARC_DTAENVGET_ERROR:
    strcat(errmsg,"dta kernel may not start.");
    break;
  case DTARC_DTAENVLOSE_ERROR:
    strcat(errmsg,"Function shmdt() failed.");
    break;
  case DTARC_FILEOPEN_ERROR:
    strcat(errmsg,"Function fopen() failed.");
    break;
  case DTARC_FILECLOSE_ERROR:
    strcat(errmsg,"Function fclose() failed.");
    break;
  case DTARC_FILEREAD_ERROR:
    strcat(errmsg,"Function fread() failed.");
    break;
  case DTARC_FILEWRITE_ERROR:
    strcat(errmsg,"Function fwrite() failed.");
    break;
  case DTARC_FILESEEK_ERROR:
    strcat(errmsg,"Function fseek() failed.");
    break;
  case DTARC_FILEFLUSH_ERROR:
    strcat(errmsg,"Function fflush() failed.");
    break;
  case DTARC_FILETRUNCATE_ERROR:
    strcat(errmsg,"Function truncate() failed.");
    break;
  case DTARC_SOCKETCREATE_ERROR:
    strcat(errmsg,"Function socket() failed.");
    break;
  case DTARC_SOCKETCONNECT_ERROR:
    strcat(errmsg,"Function connect() failed.");
    break;
  case DTARC_SOCKETCLOSE_ERROR:
    strcat(errmsg,"Function close() failed.");
    break;
  case DTARC_SOCKETREAD_ERROR:
    strcat(errmsg,"Function readn() failed.");
    break;
  case DTARC_SOCKETWRITE_ERROR:
    strcat(errmsg,"Function writen() failed.");
    break;
  case DTARC_MEMORYALLOC_ERROR:
    strcat(errmsg,"Function malloc() failed.");
    break;
  case DTARC_MEMORYMAP_ERROR:
    strcat(errmsg,"Function mmap() failed.");
    break;
  case DTARC_MEMORYUNMAP_ERROR:
    strcat(errmsg,"Function munmap() failed.");
    break;
  case DTARC_SEMCREATE_ERROR:
    strcat(errmsg,"Function semget(0) failed.");
    break;
  case DTARC_SEMOPEN_ERROR:
    strcat(errmsg,"Function semget(1) failed.");
    break;
  case DTARC_DTAPATH_ERROR:
    strcat(errmsg,"Could not get DTAPATH from environment.");
    break;
  case DTARC_SERVICEPORT_ERROR:
    strcat(errmsg,"Could not get the services port from /etc/services.");
    break;
  case DTARC_REMOTEHOSTIP_ERROR:
    strcat(errmsg,"Could not get the hostname from /etc/hosts or not correct ipaddress.");
    break;
  case DTARC_PROTOTYPE_ERROR:
    strcat(errmsg,"Not correct prototype.");
    break;
  default:;
  }
  sprintf(errmsg,"%s errno: %d message: %s",errmsg,errno,strerror(errno));
  return errmsg;
}


//-------------------------------------------------------------------
ssize_t readn(int fd,void *vptr,size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr;
  Sigfunc *sigfunc;

  ptr=(char *)vptr;
  nleft=n;
  sigfunc=signal(SIGALRM,dealalarm);
  while(nleft>0)
  {
    alarm(rcvtimeo);
    if((nread=read(fd,ptr,nleft))<0)
    {
      break;
      //if(errno==EINTR)nread=0;
      //else return -1;
    }
    else if(nread==0)break;
    nleft-=nread;
    ptr+=nread;
  }
  alarm(0);
  signal(SIGALRM,sigfunc);
  return n-nleft;
}


//-------------------------------------------------------------------
ssize_t writen(int fd,const void *vptr,size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;
  Sigfunc *sigfunc;

  ptr=(char *)vptr;
  nleft=n;
  sigfunc=signal(SIGALRM,dealalarm);
  while(nleft>0)
  {
    alarm(sndtimeo);
    if((nwritten=write(fd,ptr,nleft))<=0)
    {
      break;
      //if(errno==EINTR)nwritten=0;
      //else return -1;
    }
    nleft-=nwritten;
    ptr+=nwritten;
  }
  alarm(0);
  signal(SIGALRM,sigfunc);
  return n;
}


//-------------------------------------------------------------------
void dealalarm(int signo)
{
  signal(SIGALRM,dealalarm);
  return;
}


//-------------------------------------------------------------------
int setrlock(int semid)
{
  struct sembuf op[2];

  op[0].sem_num=1;
  op[0].sem_op=0;
  //op[0].sem_flg=SEM_UNDO;
  op[0].sem_flg=0;
  op[1].sem_num=0;
  op[1].sem_op=1;
  op[1].sem_flg=SEM_UNDO;

  if(semop(semid,op,2)<0)return -1;
  return 0;
}


//-------------------------------------------------------------------
int freerlock(int semid)
{
  struct sembuf op;

  op.sem_num=0;
  op.sem_op=-1;
  op.sem_flg=SEM_UNDO;

  semop(semid,&op,1);
  return 0;
}


//-------------------------------------------------------------------
int setwlock(int semid)
{
  struct sembuf op[3];

  op[0].sem_num=0;
  op[0].sem_op=0;
  //op[0].sem_flg=SEM_UNDO;
  op[0].sem_flg=0;
  op[1].sem_num=1;
  op[1].sem_op=0;
  //op[1].sem_flg=SEM_UNDO;
  op[1].sem_flg=0;
  op[2].sem_num=1;
  op[2].sem_op=1;
  op[2].sem_flg=SEM_UNDO;

  semop(semid,op,3);
  return 0;
}


//-------------------------------------------------------------------
int freewlock(int semid)
{
  struct sembuf op;

  op.sem_num=1;
  op.sem_op=-1;
  op.sem_flg=SEM_UNDO;

  semop(semid,&op,1);
  return 0;
}


//-------------------------------------------------------------------
int setmlock(int semid)
{
  struct sembuf op[2];

  op[0].sem_num=2;
  op[0].sem_op=0;
  //op[0].sem_flg=SEM_UNDO|IPC_NOWAIT;
  op[0].sem_flg=IPC_NOWAIT;
  op[1].sem_num=2;
  op[1].sem_op=1;
  op[1].sem_flg=SEM_UNDO;

  if(semop(semid,op,2)<0)return -1;
  return 0;
}


//-------------------------------------------------------------------
int freemlock(int semid)
{
  struct sembuf op;

  op.sem_num=2;
  op.sem_op=-1;
  op.sem_flg=SEM_UNDO;

  semop(semid,&op,1);
  return 0;
}


//-------------------------------------------------------------------
int isdirectory(char *dirname)
{
  struct stat sbuf;
  if(stat(dirname,&sbuf)<0)return -1;
  if((sbuf.st_mode & S_IFMT)==S_IFDIR)return 1;
  else return 0;
}


//-------------------------------------------------------------------
void perr_t(const char *fmt,...)
{
  va_list ap;

  va_start(ap,fmt);
  fprintf(stderr,"<%s> ",getformattime());
  vfprintf(stderr,fmt,ap);
  fprintf(stderr,"\n");
  va_end(ap);
  fflush(stderr);
  return;
}


//-------------------------------------------------------------------
void plog_t(const char *fmt,...)
{
  va_list ap;

  va_start(ap,fmt);
  fprintf(stdout,"<%s> ",getformattime());
  vfprintf(stdout,fmt,ap);
  fprintf(stdout,"\n");
  va_end(ap);
  fflush(stdout);
  return;
}

//-------------------------------------------------------------------
char *getformattime()
{
  static char sNowTime[20];
  time_t lNowTime;
  struct tm *stNowTime;

  time(&lNowTime);
  stNowTime=localtime(&lNowTime);
  strftime(sNowTime,20,"%Y-%m-%d %H:%M:%S",stNowTime);
  return sNowTime;
}


//-------------------------- The End --------------------------------

