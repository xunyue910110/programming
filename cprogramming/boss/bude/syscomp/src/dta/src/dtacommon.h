//-------------------------------------------------------------------
// Name      : dtacommon.h
// Version   : 0.2.8
// Create    : 2000.6.5
// Update    : 2002.4.4
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : Should be included in DTA kernel.
//-------------------------------------------------------------------
#ifndef DTACOMMON_H
#define DTACOMMON_H

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef INADDR_NONE
#define INADDR_NONE      ((in_addr_t)0xffffffff) /* -1 return */
#endif

#define QLEN             5
#define BUFFERLENGTH     16384
#define SMSGLEN          200
#define MAXMSGS          5000
#define QSEND_INTERVAL   5
#define QCLEAN_INTERVAL  11
#define MAXQUEUES        1000

#define RETURN_ON_FAIL(ret) {  \
      strcpy(dtaef,__FILE__);  \
      dtael=__LINE__;          \
      return ret;              \
    }

typedef struct
{
  long maxmsgs;
  long livmsgs;
  long firstmsgn;
  long lastmsgn;
}QueueHead;

typedef struct
{
  long flag;
  long offset;
  long msglen;
  long priority;
  long foreprioidx;
  long nextprioidx;
}QueueTable;

typedef struct
{
  char queuename[50];
  int queuetype;
  char remotehost[50];
  char remotequeue[50];
}QUEUELIST;

typedef struct
{
  char dtapath[100];
}DTAENV;
extern DTAENV *dtaenv;

union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};

typedef void Sigfunc(int);

#ifdef __cplusplus
extern "C"
{
#endif

  ssize_t readn(int fd,void *vptr,size_t n);
  ssize_t writen(int fd,const void *vptr,size_t n);
  int setrlock(int semid);
  int freerlock(int semid);
  int setwlock(int semid);
  int freewlock(int semid);
  int setmlock(int semid);
  int freemlock(int semid);
  int isdirectory(char *dirname);
  void perr_t(const char *fmt,...);
  void plog_t(const char *fmt,...);
  char *getformattime();
  void dealalarm(int signo);

#ifdef __cplusplus
}
#endif

#endif // DTACOMMON_H

//-------------------------- The End --------------------------------
