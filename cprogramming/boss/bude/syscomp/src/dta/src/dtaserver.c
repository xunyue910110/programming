//-------------------------------------------------------------------
// Name      : dtaserver.c
// Version    : 0.3.1
// Create    : 2000.6.5
// Update    : 2002.4.17
// Auther    : QianYiXin<qianyx@lianchuang.com>
// Copyright  : All Rights Reserved.
// Description  : DTAServer is a daemon program.
//-------------------------------------------------------------------
#include "dtacommon.h"
#include "dta.h"

#define DTASVERSION    "0.3.1"
#define USAGE      "USAGE: %s [-d] [-v]\n"

int debug;
char hostname[50];

int passivesock(const char *service,const char *transport,int qlen);
int tcpget(int hSock);
void querystatus(int);
int System(const char *command);
void  userterm(int);

//-------------------------------------------------------------------
int main(int argc,char *argv[])
{
  char service[]="DTAServer";
  struct sockaddr_in fsin;
  int msock;
  int ssock;
//  size_t alen;
//  unsigned int alen;
  int alen;
  FILE *fpipe;
  char str[200];
  int opt;
  //extern char *optarg;
  //extern int optind;
  //extern int optopt;
  //extern int opterr;
  int shmid;
  int timeout=30;
  int pid;

  signal(SIGCLD,SIG_IGN);
  signal(SIGPIPE,SIG_IGN);
  signal(SIGINT,userterm);
  signal(SIGTERM,userterm);
  signal(SIGUSR1,querystatus);
  DTA_SETOPT(DTAOPT_RCVTIMEO,&timeout,sizeof(int));
  DTA_SETOPT(DTAOPT_SNDTIMEO,&timeout,sizeof(int));
  /*
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

    if(getenv("DTAPATH")==NULL)
    {
    perr_t("environment value DTAPATH not found!");
    return -1;
    }
  */
  debug=0;
  while((opt=getopt(argc,argv,"dv"))!=-1)
  {
    switch(opt)
    {
    case 'd':
      debug=1;
      break;
    case 'v':
      fprintf(stdout,"DTAServer Version %s\n",DTASVERSION);
      exit(0);
    default:
      fprintf(stderr,USAGE,argv[0]);
      exit(-1);
    }
  }

  shmid=shmget(29999,sizeof(DTAENV),IPC_CREAT|0644);
  dtaenv=(DTAENV *)shmat(shmid,0,0);
  strncpy(dtaenv->dtapath,getenv("DTAPATH"),sizeof(dtaenv->dtapath));

  plog_t("%s startup now!",argv[0]);

  msock=passivesock(service,"tcp",QLEN);
  if(gethostname(hostname,sizeof(hostname)))
  {
    perr_t("gethostname(): %s",strerror(errno));
    return -1;
  }

  while(1)
  {
    alen=sizeof(fsin);
    //ssock=accept(msock,(struct sockaddr *)&fsin,&alen);
    ssock=accept(msock,NULL,NULL);
    //sockaddr->sockaddr_in
    if(ssock<0)
    {
      if(errno==EINTR)continue;
      perr_t("accept(): %s",strerror(errno));
      exit(-1);
    }
    switch(pid=fork())
    {
    case  0:
      if((pid=fork())<0)
      {
        perr_t("fork(): %s",strerror(errno));
        exit(-1);
      }
      if(pid>0)exit(0);

      close(msock);
      exit(tcpget(ssock));
    case -1:
      perr_t("fork(): %s",strerror(errno));
      exit(-1);
    default:
      close(ssock);
    }
    //if(waitpid(pid,NULL,0)!=pid)
    //  perr_t("waitpid(): %s",strerror(errno));
  }
  return 0;
}


//-------------------------------------------------------------------
int passivesock(const char *service,const char *transport,int qlen)
{
  u_short portbase=0;
  struct servent *pse;
  struct protoent *ppe;
  struct sockaddr_in sin;
  int s;
  int type;
  int opt = 1;
  int len = sizeof(opt);

  memset(&sin,0,sizeof(sin));
  sin.sin_family=AF_INET;
  sin.sin_addr.s_addr=INADDR_ANY;
  /*
    if(pse=getservbyname(service,transport))
    {
    sin.sin_port=htons(ntohs((u_short)pse->s_port)+portbase);
    }
    else if((sin.sin_port=htons((u_short)atoi(service)))==0)
    {
    perr_t("ServicePort error.");
    exit(-1);
    }*/
  sin.sin_port=htons(29999);
  if((ppe=getprotobyname(transport))==0)
  {
    perr_t("ProtoType error.");
    exit(-1);
  }
  if(strcmp(transport,"udp")==0)type=SOCK_DGRAM;
  else type=SOCK_STREAM;
  s=socket(PF_INET,type,ppe->p_proto);
  if(s<0)
  {
    perr_t("socket(): %s",strerror(errno));
    exit(-1);
  }
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,len);
  if(bind(s,(struct sockaddr *)&sin,sizeof(sin))<0)
    //sockaddr->sockaddr_in
  {
    perr_t("bind(): %s",strerror(errno));
    exit(-1);
  }
  if(type==SOCK_STREAM&&listen(s,qlen)<0)
  {
    perr_t("listen(): ",strerror(errno));
    exit(-1);
  }
  return s;
}


//-------------------------------------------------------------------
int tcpget(int fd)
{
  char buf[BUFFERLENGTH];
  int readsize;
  FILE *hfile;
  FILE *hctlfile;
  char tmpfile[200];
  char ctlfile[200];
  char what[200];
  char sourcefile[200];
  char targetfile[200];
  int filelength;
  struct stat filestat;
  char syscmd[200];
  DTAHQUEUE hqueue;
  char queuename[50];
  char openedqueue[50]="";
  long msgprio;
  long msglen;
  char *message;
  int startpos;
  int readtotal;
  int longvalue;
  char smsgbuf[SMSGLEN];
  time_t lastmoditime;
  int ret;
  char command[200];
  int result;
  int i;

  signal(SIGCLD,SIG_DFL);
  hqueue.fp=0;
  memset(smsgbuf,0,SMSGLEN);
  while((readsize=readn(fd,smsgbuf,SMSGLEN))>0)
  {
    strncpy(what,smsgbuf,sizeof(what));
    if(strcmp(what,"status")==0)
    {
      writen(fd,hostname,strlen(hostname));
    }
    if(strcmp(what,"putfile")==0)
    {
      memset(smsgbuf,0,SMSGLEN);
      readsize=readn(fd,smsgbuf,SMSGLEN);
      strncpy(sourcefile,smsgbuf,sizeof(sourcefile));
      memset(smsgbuf,0,SMSGLEN);
      readsize=readn(fd,smsgbuf,SMSGLEN);
      strncpy(targetfile,smsgbuf,sizeof(targetfile));
      if(debug)plog_t("putfile:%s",targetfile);
      if(strrchr(targetfile,'/')==NULL)
      {
        sprintf(tmpfile,"%s/tmp/%s",dtaenv->dtapath,targetfile);
      }
      else
      {
        sprintf(tmpfile,"%s/tmp/%s",dtaenv->dtapath,
                strrchr(targetfile,'/')+1);
      }
      readn(fd,&longvalue,sizeof(int));
      filelength=ntohl(longvalue);
      readn(fd,&longvalue,sizeof(time_t));
      lastmoditime=ntohl(longvalue);
      sprintf(ctlfile,"%s.ctl",tmpfile);
      if(stat(tmpfile,&filestat))
      {
        startpos=0;
      }
      else
      {
        startpos=filestat.st_size;
      }
      if(debug)plog_t("startpos:%ld\n",startpos);
      longvalue=htonl(startpos);
      writen(fd,&longvalue,sizeof(int));
      if((hfile=fopen(tmpfile,"a+b"))==NULL)
      {
        writen(fd,"0",1);
        perr_t("File %s could not open reason: %s",tmpfile,
               strerror(errno));
        return -1;
      }
      readtotal=startpos;
      while(readtotal<filelength)
      {
        readsize=readn(fd,buf,sizeof(buf)>filelength-readtotal?
                       filelength-readtotal:sizeof(buf));
        if(readsize<=0)
        {
          writen(fd,"0",1);
          fclose(hfile);
          perr_t("readn(): %s",strerror(errno));
          return -1;
        }
        if(fwrite(buf,1,readsize,hfile)!=readsize)
        {
          writen(fd,"0",1);
          fclose(hfile);
          perr_t("fwrite(): %s",strerror(errno));
          return -1;
        }
        fflush(hfile);
        readtotal+=readsize;
      }
      fclose(hfile);
      sprintf(syscmd,"mv -f %s %s",tmpfile,targetfile);
      if(System(syscmd))
      {
        writen(fd,"0",1);
        perr_t("system(): %s",strerror(errno));
        return -1;
      }
      writen(fd,"1",1);
    }
    if(strcmp(what,"getfile")==0)
    {
      memset(smsgbuf,0,SMSGLEN);
      readn(fd,smsgbuf,SMSGLEN);
      strncpy(sourcefile,smsgbuf,sizeof(sourcefile));
      if(debug)plog_t("getfile:%s",sourcefile);
      readn(fd,&longvalue,sizeof(int));
      startpos=ntohl(longvalue);
      if(debug)plog_t("startpos:%ld",startpos);
      if(stat(sourcefile,&filestat))
      {
        filelength=-1;
        longvalue=htonl(filelength);
        writen(fd,&longvalue,sizeof(int));
        return -1;
      }
      filelength=filestat.st_size;
      longvalue=htonl(filelength);
      writen(fd,&longvalue,sizeof(int));
      if((hfile=fopen(sourcefile,"rb"))==NULL)
      {
        perr_t("File %s could not open reason: %s",sourcefile,
               strerror(errno));
        return -1;
      }
      fseek(hfile,startpos,SEEK_SET);
      while(!feof(hfile))
      {
        readsize=fread(buf,1,BUFFERLENGTH,hfile);
        if(readsize==0)break;
        writen(fd,buf,readsize);
      }
      fclose(hfile);
      if(debug)plog_t("end");
    }
    if(strcmp(what,"execrcmd")==0)
    {
      memset(smsgbuf,0,SMSGLEN);
      readn(fd,smsgbuf,SMSGLEN);
      strncpy(command,smsgbuf,sizeof(command));
      if(debug)plog_t("execrcmd:%s",command);
      readn(fd,&longvalue,sizeof(int));
      result=ntohl(longvalue);
      if(debug)plog_t("result:%ld",result);


      sprintf(tmpfile,"%s/tmp/rcmd%d",dtaenv->dtapath,getpid());
      sprintf(syscmd,"%s > %s",command,tmpfile);
      ret=System(syscmd);
      longvalue=htonl(ret);
      writen(fd,&longvalue,sizeof(int));

      if((hfile=fopen(tmpfile,"r"))!=NULL)
      {
        i=0;
        while(i<result)
        {
          if(fgets(smsgbuf,200,hfile)==NULL)break;
          if(smsgbuf[strlen(smsgbuf)-1]=='\n')
            smsgbuf[strlen(smsgbuf)-1]=0;
          writen(fd,smsgbuf,SMSGLEN);
          i++;
        }
        fclose(hfile);
        strncpy(smsgbuf,"DTA:end of result.",sizeof(smsgbuf));
        writen(fd,smsgbuf,SMSGLEN);
        sprintf(syscmd,"rm -f %s",tmpfile);
        if(System(syscmd))
        {
          perr_t("system(): %s",strerror(errno));
          return -1;
        }
      }
    }
    if(strcmp(what,"sendmsg")==0)
    {
      memset(smsgbuf,0,SMSGLEN);
      writen(fd,"1",1);//+
      readsize=readn(fd,smsgbuf,SMSGLEN);
      strncpy(queuename,smsgbuf,sizeof(queuename));
      if(debug)plog_t("queuename=[%s]",queuename);
      writen(fd,"1",1);//+
      readsize=readn(fd,smsgbuf,SMSGLEN);
      sscanf(smsgbuf,"%10ld %10ld",&msgprio,&msglen);
      if(debug)plog_t("msgprio=[%d]",msgprio);
      if(debug)plog_t("msglen=[%ld]",msglen);
      if((message=(char *)malloc(msglen))==NULL)
      {
        perr_t("malloc(): %s",strerror(errno));
        return -1;
      }
      writen(fd,"1",1);//+
      readsize=readn(fd,message,msglen);
      if(debug)plog_t("readsize=%ld",readsize);
      if(debug)plog_t("msg:%s",message);

      if(strcmp(openedqueue,queuename))
      {
        if(hqueue.fp)DTA_CLOSEQ(hqueue);
        if(ret=DTA_OPENQ(&hqueue,queuename))
        {
          perr_t("DTA_OPENQ(): %s",DTA_CHKERR(ret));
          free(message);
          writen(fd,"0",1);
          return -1;
        }
        strncpy(openedqueue,queuename,sizeof(openedqueue));
      }
      if(DTA_PUTMSG(hqueue,message,msglen,msgprio))
      {
        perr_t("DTA_PUTMSG(): %s",DTA_CHKERR(ret));
        DTA_CLOSEQ(hqueue);
        free(message);
        writen(fd,"0",1);
        return -1;
      }

      free(message);
      writen(fd,"1",1);
      //send(fd,"1",1,MSG_OOB);
    }
  }
  if(readsize<0)perr_t("readn(): %s",strerror(errno));
  close(fd);
  if(hqueue.fp)DTA_CLOSEQ(hqueue);
  return 0;
}

void querystatus(int signo)
{
  signal(SIGUSR1,querystatus);
  plog_t("dtaserver status: %s");
}

int System(const char *cmdstring)
{
  pid_t pid;
  int status;

  struct sigaction ignore,saveintr,savequit;
  sigset_t chldmask,savemask;

  if(cmdstring==NULL)return(1);

  ignore.sa_handler=SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  ignore.sa_flags=0;
  if(sigaction(SIGINT,&ignore,&saveintr)<0)return(-1);
  if(sigaction(SIGQUIT,&ignore,&savequit)<0)return(-1);

  sigemptyset(&chldmask);
  sigaddset(&chldmask,SIGCHLD);
  if(sigprocmask(SIG_BLOCK,&chldmask,&savemask)<0)return(-1);

  if((pid=fork())<0)
  {
    status=-1;
  }
  else if(pid==0)
  {
    sigaction(SIGINT,&saveintr,NULL);
    sigaction(SIGINT,&savequit,NULL);
    sigprocmask(SIG_SETMASK,&savemask,NULL);

    execl("/bin/sh","sh","-c",cmdstring,(char *)0);
    _exit(127);
  }
  else
  {
    //sigfunc=signal(SIGALRM,dealalarm);
    //alarm(30);
    while(waitpid(pid,&status,0)<0)
    {
      if(errno!=EINTR)
      {
        status=-1;
        break;
      }
    }
    //  signal(SIGALRM,sigfunc);
  }

  if(sigaction(SIGINT,&saveintr,NULL)<0)return(-1);
  if(sigaction(SIGQUIT,&savequit,NULL)<0)return(-1);
  if(sigprocmask(SIG_SETMASK,&savemask,NULL)<0)return(-1);

  return(status);
}

void userterm(int a)
{
  perr_t("terminated by user.");
  exit(-1);
}

//-------------------------- The End --------------------------------
