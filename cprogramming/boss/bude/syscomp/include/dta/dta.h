//-------------------------------------------------------------------
// Name      : dta.h
// Version   : 0.2.8
// Create    : 2000.6.5
// Update    : 2002.4.4
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : Should be included in DTA application.
//-------------------------------------------------------------------
#ifndef DTA_H
#define DTA_H

#include <stdio.h>

#define DTAGMO_READBYORDER         0
#define DTAGMO_READBYPRIORITY      1
#define DTAGMO_SKIPWHILEREAD       2
#define DTAGMO_DELETEAFTERREAD     4
#define DTAGMO_RESETCURSOR         8
#define DTAGMO_JUSTINQUIRE         16
#define DTA_QTYPE_RECEIVE          0
#define DTA_QTYPE_SEND             1
#define DTAOPT_CONNTIMEO           0
#define DTAOPT_RCVTIMEO            1
#define DTAOPT_SNDTIMEO            2


#define DTARC_OK                   0

#define DTARC_QUEUE_EMPTY          1011
#define DTARC_QUEUE_FULL           1012
#define DTARC_QUEUE_END            1013
#define DTARC_MESSAGE_DIED         1014

#define DTARC_GENERAL_ERROR        2001
#define DTARC_INTERNAL_ERROR       2002
#define DTARC_DTAENV_ERROR         2003
#define DTARC_DTAENVGET_ERROR      2004
#define DTARC_DTAENVLOSE_ERROR     2005

#define DTARC_FILEOPEN_ERROR       2011
#define DTARC_FILECLOSE_ERROR      2012
#define DTARC_FILEREAD_ERROR       2013
#define DTARC_FILEWRITE_ERROR      2014
#define DTARC_FILESEEK_ERROR       2015
#define DTARC_FILEFLUSH_ERROR      2016
#define DTARC_FILETRUNCATE_ERROR   2017

#define DTARC_SOCKETCREATE_ERROR   2021
#define DTARC_SOCKETCONNECT_ERROR  2022
#define DTARC_SOCKETCLOSE_ERROR    2023
#define DTARC_SOCKETREAD_ERROR     2024
#define DTARC_SOCKETWRITE_ERROR    2025

#define DTARC_MEMORYALLOC_ERROR    2031
#define DTARC_MEMORYMAP_ERROR      2032
#define DTARC_MEMORYUNMAP_ERROR    2033
#define DTARC_SEMCREATE_ERROR      2041
#define DTARC_SEMOPEN_ERROR        2042
#define DTARC_SYSCMD_ERROR         2051
#define DTARC_EXECRCMD_ERROR       2061

#define DTARC_DTAPATH_ERROR        2111
#define DTARC_SERVICEPORT_ERROR    2112
#define DTARC_REMOTEHOSTIP_ERROR   2113
#define DTARC_PROTOTYPE_ERROR      2114


typedef struct
{
  int fd;
}DTAHCONN;

typedef struct
{
  char queuename[50];
  char fullfilename[150];
  FILE *fp;
  void *tablemap;
  long tablelen;
  long cursor;
  int semid;
}DTAHQUEUE;

typedef struct
{
  long filesize;
  long startpos;
}DTAFTINFO;

#ifdef __cplusplus
extern "C"
{
#endif

  int DTA_INIT();
  int DTA_EXIT();

  int DTA_CONN(DTAHCONN *hconn,const char *host);
  int DTA_DISC(DTAHCONN hconn);
  int DTA_PUT(DTAHCONN hconn,const char *sourcefile,const char *targetfile,
              DTAFTINFO *ftinfo);
  int DTA_GET(DTAHCONN hconn,const char *sourcefile,const char *targetfile,
              DTAFTINFO *ftinfo);

  int DTA_RESULT(char *array,int length);
  int DTA_RCMD(DTAHCONN hconn,const char *command,int result,
               int *resultrow);

  int DTA_OPENQ(DTAHQUEUE *hqueue,const char *queuename);
  int DTA_CLOSEQ(DTAHQUEUE hqueue);
  int DTA_ASKQ(DTAHQUEUE hqueue,long *maxmsgs,long *livmsgs);
  int DTA_EMPTYQ(DTAHQUEUE hqueue);
  int DTA_CLEANQ(DTAHQUEUE hqueue);

  int DTA_PUTMSG(DTAHQUEUE hqueue,const char *message,long msglen,
                 long priority);
  int DTA_GETMSG(DTAHQUEUE *hqueue,char *message,long *msglen,
                 long *priority,int mode);
  int DTA_DELMSG(DTAHQUEUE hqueue,long msgn);
  int DTA_SENDMSG(DTAHCONN hconn,const char *remotequeue,
                  const char *message,long msglen,long priority);
  char *DTA_CHKERR(int err);

  int DTA_SETOPT(int optionname,const void *optionvalue,
                 size_t optionlength);
  int DTA_GETOPT(int optionname,void *optionvalue,size_t *optionlength);

#ifdef __cplusplus
}
#endif

#endif  // DTA_H

//-------------------------- The End --------------------------------
