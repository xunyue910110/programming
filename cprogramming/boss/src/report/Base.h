// Base.h: interface for the CBase class.
//
//////////////////////////////////////////////////////////////////////
#ifndef BASE_H
#define BASE_H
//#define MMV_HP


#include "config-all.h"
#include        <sys/statvfs.h>
#include        <unistd.h>
#include        <sys/types.h>
#include        <sys/ipc.h>
#include        <sys/errno.h>
#include        <string.h>
#include        <sys/shm.h>
#include        <stdio.h>
#include        <strings.h>
#include        <stdlib.h>
#include        <sys/stat.h>
#include        <netdb.h>
#include        <dirent.h>
#include        <ctype.h>
#include        <sys/socket.h> 
#include        <netinet/in.h>
#include        <arpa/inet.h>
#include        <sys/time.h> 
#include        <sys/file.h>
#include        <sys/msg.h>
#include        <time.h>
//#include        <sys/vfs.h>
#include        <string>
#include        <signal.h>
#include        <setjmp.h>
#include        <ctype.h>
#include       <sys/time.h>

#ifdef MMV_HP
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/vfs.h>
#endif


#ifdef MMV_SUN
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/statfs.h>
#endif

/*定义消息类型*/
#define LOG_MSG    1      /*日志消息*/
#define ALERT_MSG  2      /*告警消息*/
#define STATE_MSG  3      /*状态消息*/
#define SYS_MSG    4      /*系统消息*/

//#define CONFIGNAME "../config/mmv.ini"
//#define CFG_HOSTSPACE "../config/mmv_diskspace.ini"
//#define STATECONFIGGILE "../config/state.ini"

extern char* getFullCfgPath(const char*);

#define CONFIGNAME      getFullCfgPath("mmv.ini")
#define CFG_HOSTSPACE   getFullCfgPath("mmv_diskspace.ini")
#define STATECONFIGGILE getFullCfgPath("state.ini")

USING_NAMESPACE(std)
#define MAXPIDS 36


extern int errno;
struct ProcList
{
        int procs;
        int pidlist[MAXPIDS];
};

class CBase  
{
public:
	
	CBase();
	virtual ~CBase();
	int rtrim(char *s);
	int ltrim(char *s);
	int mtrim(char *s);
	int CheckProc(char *proc_name);
	int CheckProc1(char *proc_name);//检查非正常退出的进程
	int isprocactive(char *procname,int exclpid);
	int getpidbyname(char *procname,struct ProcList *plist);
	int getcdate(char *date);
	int getctime(char *t);
	int IsDirectory(char *dirname);
	int GetProfile(char *fname,char *segname,char *configname,char *value);
	int CheckSUNHP_FSFreeSpace(const char *fspath,float percentlimit);	
};

#endif 
