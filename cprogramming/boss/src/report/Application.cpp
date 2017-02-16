// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
//
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
//
// Comments:
//

#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <memory.h>
#include <errno.h>
#include <sys/stat.h>

#include "config-all.h"

#ifdef WIN32_OS_CONFIG  // for _getpid()
#include <process.h>
#else
#include <unistd.h>
#endif

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <strstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <strstream>
#include <iostream>
#endif


#include "Application.h"
#include "SysParam.h"
#include "ConfigureFile.h"
#include "ErrorHandle.h"
#include "ServiceMonitor.h"
/*
#include "Alert.h"
#include "Log.h"
#include "State.h"
*/
#include "ServiceMonitor.h"


#ifndef GETOPT_COMPATIBLE
#define GETOPT_COMPATIBLE
#endif //GETOPT_COMPATIBLE

// enable dump alert message to monitor for debug purpose

//#ifndef DUMP_ALERT_TO_MONITOR
//#define DUMP_ALERT_TO_MONITOR
//#endif //DUMP_ALERT_TO_MONITOR
#undef DUMP_ALERT_TO_MONITOR


// global Application object instance, which will replaced by your application
//Application g_application;
//Application * const theApp = &g_application;

// global ErrorMessages object
ErrorMessages g_errormessages;
ErrorMessages *const theErrorMessages = &g_errormessages;


// predefined SYSTEM parameters section and name
const char *SECT_SYSTEM = "\\SYSTEM";
//const char *SECT_FIELDS  = "FIELDS";

// the follow key's section path is SECT_SYSTEM
const char *KEY_LOCKPATH = "SYS_LOCKPATH";
// the follow keys' section path is SECT_SYSTEM + '\\' + app_name
//const char *KEY_LOGID = "LOG_ID";
//const char *KEY_EXECPATH = "EXEC_PATH";
//const char *KEY_EXECARGV = "EXEC_ARGV";
//const char *KEY_SYSNUMBER = "SYS_NUMBER";
//const char *KEY_TABLE_NAME = "TABLE_NAME";
//const char *KEY_FIELD_CAPTION = "CAPTION";
const char *STATE_NOOBJECT = "----------";

enum PROCSTATE
{
  PROCSTATE_READY = 0,
  PROCSTATE_RUN,
  PROCSTATE_EXIT,
  PROCSTATE_ABORT,
  PROCSTATE_WAIT,
  PROCSTATE_IDLE,
  PROCSTATE_FATAL,
  PROCSTATE_WARN,
  PROCSTATE_INITING,
  PROCSTATE_INITINITFAILURE,
  PROCSTATE_RECOVERYING,
  PROCSTATE_RECOVERYFAILURE
};

const char *STATE_RESERVED[] = {
  "Application READY",
  "Application RUNNING",
  "Application EXITED",
  "Application ABORT",
  "Application WAITTING",
  "Application IDLE",
  "Application FATAL",
  "Application WARN",
  "Application INITTING",
  "Application INIT FAILURE",
  "Application RECOVERYING",
  "Application RECOVERY FAILURE"
};


enum START
{
  START_INITOK = 0,
  START_INITFAIL,
  PROCSTATE_RECOVERYOK,
  PROCSTATE_RECOVERYFAIL,
  PROCSTATE_RUNNING
};

const char *START_MSG[] = {
  "APPLICATION STARTUP -- initialization complete!",
  "APPLICATION STARTUP -- initialization failure!",
  "APPLICATION STARTUP -- recovery complete!",
  "APPLICATION STARTUP -- recovery failure!",
  "APPLICATION STARTUP -- enter main loop!"
};

#define PROC_PAUSE       '^'  /*暂停*/
#define COMMAND_CONTINUE 'C'
#define COMMAND_STOP     'S'
#define COMMAND_PAUSE    'P'


//##ModelId=3BCAC96401BD
volatile char Application::m_curcmd = COMMAND_CONTINUE;

//##ModelId=3BCD16D9016B
Application::Application(): m_optind(1), m_sp(1), m_channelNo(0),
  m_onlyone(false), m_asdaemon(false), m_runinbkg(false), m_stop(0)
{
  m_objectName = STATE_NOOBJECT;
  m_reserved = STATE_RESERVED[PROCSTATE_READY];
  m_status = PROC_READY;  

  m_theErrorMessages = theErrorMessages;
  m_theErrorMessages->clean();
}


//##ModelId=3BCB1FE703A5
bool Application::processopt(int optopt, const char *optarg)
{
  string emsg;
  bool rc = true;

  // process opt
  switch (optopt)
  {
  case 0:
    break;

  case 'c':
    m_channelNo = atoi(optarg);
    break;

  case ':':
    emsg = "Option -";
    emsg = emsg + m_optopt + " requires an argument";
    m_theErrorMessages->insert(EAPPFRM_ARG, emsg);
    rc = false;
    break;

  case '?':
    emsg = "Unrecognized option: - ";
    m_theErrorMessages->insert(EAPPFRM_ARG, emsg + m_optopt);
    rc = false;
    break;

  default:
    break;
  }
  return rc;
}

//##ModelId=3B5E710D021F
bool Application::initialization()
{
  if (!parseOpt())
    return false;

  // init ErrorMessages
  m_theErrorMessages->setApplicationInfo(m_name, m_channelNo);

  // init SysParam
  if ((m_theSysParam = new ConfigureFile()) == 0)
  {
    m_theErrorMessages->insert(EAPPFRM_SYSINIT, "new ConfigureFile");
    return false;
  }

  if (!m_theSysParam->initialization())
  {
    m_theErrorMessages->insert(EAPPFRM_SYSINIT, "SysParam init");
    return false;
  }

  string section, value;
  section = SECT_SYSTEM;
/*
  if (!m_theSysParam->openSection(section))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, section);
    return false;
  }
*/

  if (!m_theSysParam->getValue(section, KEY_LOCKPATH, m_lockpath))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, section + SECTDELIM + KEY_LOCKPATH);
    return false;
  }
/*
  // set log id
  section = section + SECTDELIM + m_name;
  if (!m_theSysParam->getValue(section, KEY_LOGID, value))
  {
    m_logTypeId = -1;
  }
  else
  {
    m_logTypeId = atoi(value.c_str());

}
*/
  m_logTypeId = -1;

  if((m_theMonitor = new ServiceMonitor(m_argc, m_argv, m_lockpath)) == 0)
  {
    m_theErrorMessages->insert(EAPPFRM_SYSINIT, "new ServiceMonitor error");
    return false;
  }

  if(!m_theMonitor->initialize())
  {
    m_theErrorMessages->insert(EAPPFRM_SYSINIT, "ServiceMonitor initialize.");
    return false;
  }

  m_reserved = STATE_RESERVED[PROCSTATE_INITING];
  updateStatus();

  return true;
}


//##ModelId=3B5E74DE03C8
bool Application::recovery()
{
  m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,
                             "recovery ... (TO BE DETERMINED)");
  m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,
                             START_MSG[PROCSTATE_RECOVERYOK]);
  return true;
}

//##ModelId=3B5E71200027
bool Application::end()
{
  m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,  "Application is end");
  writeAlert();
  
  m_objectName = "";
  m_reserved = "";
  m_status = PROC_EXIT;
  updateStatus();

  m_theMonitor->end();
  m_theSysParam->end();
  
  delete m_theMonitor;
  delete m_theSysParam;
  
  return true;
}

//##ModelId=3BCA570C0053
bool Application::loopProcess()
{
  return true;
}

//##ModelId=3B5E7146036B
int Application::run()
{
  char errorNo[10];
  sprintf(errorNo, "%04d", E_SYSNOTIFY_STARTUP);

  // application initialization
  if (!initialization())
  {
    m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP, START_MSG[START_INITFAIL]);
    writeAlert();
    /*dumpstream << "ALERT: " << errorNo << " - "
               << START_MSG[START_INITFAIL] << endl;

    while (!m_theErrorMessages->isEmpty())
    {
      ERRORINFO errorInfo;
      m_theErrorMessages->get(errorInfo);
      dumpstream << "ECODE-" << errorInfo.no << ": "
                 << errorInfo.message << endl;
    }
    */
    m_reserved = STATE_RESERVED[PROCSTATE_INITINITFAILURE];
    updateStatus();
    return -1;
  }

  
  if(!m_theMonitor->alert(E_SYSNOTIFY_STARTUP, START_MSG[START_INITOK]))
  {
    dumpstream << "WRITE ALERT ERROR, CHECH YOUR ALERT SYSTEM CONFIG!" << endl;
    m_status = PROC_RUN;
    m_reserved = STATE_RESERVED[PROCSTATE_INITINITFAILURE];
    updateStatus();
    return -1;
  }

#ifdef DUMP_ALERT_TO_MONITOR
  dumpstream << "ALERT: " << errorNo << " - " << emsg << endl;
#endif //DUMP_ALERT_TO_MONITOR

  writeAlert();

  m_status = PROC_READY;
  m_reserved = STATE_RESERVED[PROCSTATE_RECOVERYING];
  updateStatus();

  if (!recovery())
  {
    writeAlert();
    m_status = PROC_RUN;
    m_reserved = STATE_RESERVED[PROCSTATE_RECOVERYFAILURE];
    updateStatus();
    return -1;
  }

  preLoop();

  // main loop
  mainLoop();

  // application exit
  end();

  return 0;
}


//##ModelId=3BCA7EEC00FC
void Application::setopt(int argc, char **argv)
{
  // get application name from command line
  char * str;
#ifdef WIN32_OS_CONFIG
  if ((str = strrchr(argv[0], '\\')) == 0)
#else
  if ((str = strrchr(argv[0], '/')) == 0)
#endif
    str = argv[0];
  else
    str++;
  if (strlen(str) <= MAXAPPNAME)
    strcpy(m_name, str);

   m_argc = argc;
   m_argv = argv;
}


//##ModelId=3BCD16D901ED
void Application::setoptstr(const char *optstring)
{
  assert(strlen(optstring) <= MAXAPPNAME);
  strcpy(m_optstring, optstring);
}


//##ModelId=3BCA79B701A9
int Application::getopt(char *opts)
{
  int c;
  char *cp;

  if (m_sp == 1)
  if (m_optind >= m_argc ||
      m_argv[m_optind][0] != '-' || m_argv[m_optind][1] == '\0')
    return ( -1);
  else if (strcmp(m_argv[m_optind], "--") == 0)
  {
    m_optind++;
    return ( -1);
  }

  m_optopt = c = m_argv[m_optind][m_sp];
  if (c == ':' || (cp=strchr(opts, c)) == NULL)
  {
    if(m_argv[m_optind][++m_sp] == '\0')
    {
      m_optind++;
      m_sp = 1;
    }
    return('?');
  }

  if(*++cp == ':')
  {
    if(m_argv[m_optind][m_sp+1] != '\0')
      m_optarg = &m_argv[m_optind++][m_sp+1];
    else if(++m_optind >= m_argc)
    {
      m_sp = 1;
      return(':');
    }
    else
      m_optarg = m_argv[m_optind++];
    m_sp = 1;
  }
  else
  {
    if(m_argv[m_optind][++m_sp] == '\0')
    {
      m_sp = 1;
      m_optind++;
    }
    m_optarg = NULL;
  }
  return(c);
}


//##ModelId=3BCD16DA0036
bool Application::parseOpt()
{
#ifdef GETOPT_COMPATIBLE
  int c;
  string emsg;
  while ((c = getopt(m_optstring)) != -1)
  {
    if (!processopt(c, m_optarg))
      return false;
  }
  if (m_optind < m_argc)
  {
    emsg = "argument error: ";
    emsg = emsg + m_argv[m_optind];
    m_theErrorMessages->insert(EAPPFRM_ARG, emsg);
    return false;
  }
  return true;
#else
  if (m_argc > 2)
  {
    m_theErrorMessages->insert(EAPPFRM_ARG,
                               "too manny argument, only support channelNo!");
    return false;
  }
  else if (m_argc == 2)
  {
    char * ch = m_argv[1];
    while (*ch)
    {
      if (!isdigit(*ch++))
      {
        m_theErrorMessages->insert(EAPPFRM_ARG,
                                   "argument channelNo must be digital!");
        return false;
      }
    }
    m_channelNo = atoi(m_argv[1]);
  }
  return true;
#endif
}


//##ModelId=3BCB7A4A03D1
void Application::writeAlert()
{
  while (!m_theErrorMessages->isEmpty())
  {
    char errorNo[10];

    ERRORINFO errorInfo;
    m_theErrorMessages->get(errorInfo);
    errorNo[0] = '\0';
    sprintf(errorNo, "%04d", errorInfo.no);
#ifdef DUMP_ALERT_TO_MONITOR
    dumpstream << "ALERT: " << errorNo << " - " << errorInfo.message << endl;
#endif //DUMP_ALERT_TO_MONITOR
    if(m_theMonitor->alert(errorInfo.no, errorInfo.message))
    {
#ifdef DUMP_ALERT_TO_MONITOR
      dumpstream << "WRITE ALERT ERROR, CHECH YOUR ALERT SYSTEM CONFIG!"
                 << endl;
#endif //DUMP_ALERT_TO_MONITOR
    }
  }
}

//##ModelId=3BCC2CB4031C
void Application::updateStatus()
{
  static NameValueList status;
    
  status.resize(4);
 
  const char *stat = NULL;
  
  switch(m_status)
  {
    case PROC_READY:
      stat = "准备";
      break;
    case PROC_RUN:
      stat = "运行";
      break;
    case PROC_EXIT:
      stat = "已退出";
      break;
    case PROC_ABORT:
      stat = "异常退出";
      break;
    case PROC_WAIT:
      stat = "等待";
      break;
    case PROC_IDLE:
      stat = "空闲";
      break;
    case PROC_FATAL:
      stat = "致命错误";
      break;
    case PROC_WARN:
      stat = "告警";
      break;
    case PROC_PAUSE:
      stat = "暂停";
      break;  
    default:
      stat = "未知";
      break;
  }
  
  //判断状态是否变化
  if(strcmp(stat, status[0].m_value.c_str()) == 0 &&
     m_objectName == status[1].m_value &&
     m_reserved   == status[2].m_value)
  {
     return;
  }
 
  time_t l_time = time(NULL);
  tm *l_tm = localtime(&l_time);
  
  char buffer[32];
  sprintf(buffer, "%02d-%02d %02d:%02d:%02d", l_tm->tm_mon+1, l_tm->tm_mday, l_tm->tm_hour, l_tm->tm_min, l_tm->tm_sec);
  
  status[0].m_name = "应用状态";
  status[0].m_value = stat;
  status[1].m_name = "处理对象";
  status[1].m_value = m_objectName;
  status[2].m_name = "备注";
  status[2].m_value = m_reserved;
  status[3].m_name = "更新时间";
  status[3].m_value = buffer;
  m_theMonitor->setStatus(status);
}

void Application::ssleep(int sec)
{  
  for(char lastcmd = m_curcmd; sec-- > 0; )
  {
    sleep(1);
    if(lastcmd != m_curcmd)
      break;
  }
}

//##ModelId=3BCB910D0300
bool Application::get_asdaemon() const
{
  return m_asdaemon;
}
//##ModelId=3BCB910D038C
void Application::set_asdaemon(bool left)
{
  m_asdaemon = left;
}

//##ModelId=3BCB910E00EF
bool Application::get_onlyone() const
{
  return m_onlyone;
}
//##ModelId=3BCB910E017B
void Application::set_onlyone(bool left)
{
  m_onlyone = left;
}

//##ModelId=3BCB924A022A
bool Application::get_runinbkg() const
{
  return m_runinbkg;
}
//##ModelId=3BCB924A02CA
void Application::set_runinbkg(bool left)
{
  m_runinbkg = left;
}


//##ModelId=3BCB97520181
void Application::preLoop()
{
  char channel[10];
  sprintf(channel, "%04d", m_channelNo);
  m_lockpath = m_lockpath + m_name + channel;

  if (m_asdaemon)
    fork_child();

  // child process
  if (m_asdaemon || m_runinbkg)
    to_background();

  if (m_onlyone)
    run_onlyone(m_lockpath.c_str());

  // process SIGTERM signal
  signal(SIGTERM, &Application::term_proc);
  signal(SIGINT,  &Application::term_proc);
  signal(SIGSTOP,  &Application::term_proc);
  signal(SIGCONT,  &Application::term_proc);  

  // get pid
#ifndef WIN32_OS_CONFIG
  m_pid = getpid();
#else
  m_pid = _getpid();
#endif

  // close all opened section
  m_theSysParam->closeAllSection();

/*
  // save currentpath, recover at end
  getcwd(m_currentpath, CHNLPATH)
  // chang to workpath
  chdir(m_workpath);
  // set mask mode and save old mask for recorvery
  m_omask = m_umask(nmask_mode);
*/
}

//##ModelId=3BCB132E0329
int Application::mainLoop()
{
  m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP, START_MSG[PROCSTATE_RUNNING]);
  writeAlert();

  // update status on startup
  m_status = PROC_RUN;
  m_reserved = STATE_RESERVED[PROCSTATE_RUN];
  updateStatus();
      
  while(!m_stop)
  {
    char cmd = m_curcmd;
    
    if(cmd == COMMAND_STOP)
       break;
        
    else if(cmd == COMMAND_CONTINUE)
    {       
      loopProcess();

      // update status
      updateStatus();
      
      // write process Log
      writeLog();
      
      // write alert
      writeAlert();
    }
    
    else //if(cmd == COMMAND_PAUSE)
    {
      m_status = PROC_PAUSE;
      updateStatus();
      sleep(1);
      continue;
    }
  }
  
  
  return 0;
}

void Application::writeLog()
{
  if (m_logstr.size())
  {
     m_theMonitor->log(m_logTypeId, m_logstr.c_str());
     m_logstr = "";
  }
}

void Application::writeLog(int typeId, const string& str)
{
  m_theMonitor->log(typeId, str.c_str());
}

void Application::writeLog(int typeId, const char*   str)
{
  m_theMonitor->log(typeId, str);
}

//##ModelId=3BCAC96402CB
void Application::term_proc(int sig)
{
  switch(sig)
  {
    case SIGINT:
    case SIGTERM:
        m_curcmd = COMMAND_STOP;        
        theApp->m_stop = true;
        break;
    case SIGSTOP:
        if(m_curcmd == COMMAND_CONTINUE)
          m_curcmd = COMMAND_PAUSE;
        break;
    case SIGCONT:
        if(m_curcmd == COMMAND_PAUSE)
          m_curcmd = COMMAND_CONTINUE;
        break;
  } 
  // reset signal process with myself
  signal(sig, &Application::term_proc);
}

void Application::fork_child()
{
#ifndef WIN32_OS_CONFIG
  // fork child process
  long pid = fork();
  if (pid == 0)
  { // child process, continue
  }
  else if (pid > 0)
  { // parent process
    exit(0);
  }
  else
  { // fork error
    exit(-1);
  }
#endif //!WIN32_OS_CONFIG
}

//##ModelId=3BCB94CC02C3
void Application::to_background()
{
#ifndef WIN32_OS_CONFIG
  // running as daemon
  // COMMENTS: using sun workshorp c++ 6 update 2 for sun os 5.8
  //  				 close(0 or 1 or 2), then using fstream object as :
  //					fstream in;
  //					in.open    -OK  first time
  //					in.close   -OK  first time
  //					in.open    -FAILURE
  //					in.close
/*
  close(0);
  close(1);
  close(2);
  setpgrp();
*/

  bool rc = true;
  setsid();
  int fd = open("/dev/null", O_RDWR);
  if(fd < 0) {
  	rc = false;
  }
  if (dup2(fd, STDIN_FILENO) != STDIN_FILENO)
  	rc = false;
  if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO)
  	rc = false;
  if (dup2(fd, STDERR_FILENO) != STDERR_FILENO)
  	rc = false;

  if (fd > STDERR_FILENO)
    close(fd);
	//return rc;
#endif //!WIN32_OS_CONFIG
}


// prevent the application have two instances
//##ModelId=3BCB956202E7
int Application::run_onlyone(const char *filename)
{
#ifndef WIN32_OS_CONFIG
  int  fd, val;
  char buf[10];

  if ((fd = open(filename, O_WRONLY | O_CREAT,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
    return -1;

  // try and set a write lock on the entire file
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLK, &lock) < 0)
  {
    if (errno == EACCES || errno == EAGAIN)
      exit(0);     // gracefully exit, daemon is already running
    else
      return -1;
  }

  // truncate to zero length, now that we have the lock
  if (ftruncate(fd, 0) < 0)
    return -1;

  // and write our process ID
  sprintf(buf, "%d\n", getpid());
  if (write(fd, buf, strlen(buf)) != strlen(buf))
    return -1;

  // set close-on-exec flag for descriptor
  if ( (val = fcntl(fd, F_GETFD, 0)) < 0)
    return -1;
  val |= FD_CLOEXEC;
  if (fcntl(fd, F_SETFD, val) < 0)
    return -1;

  // leave file open until we terminate: lock will be held
#endif //!WIN32_OS_CONFIG
  return 0;
}



int main(int argc, char *argv[])
{
  theApp->setopt(argc, argv);
  string sTemp = argv[0];
  string sTemp1 = sTemp.substr(sTemp.find_last_of('/')+1,100);
  strcpy(argv[0],sTemp1.c_str());
  return theApp->run();
}

