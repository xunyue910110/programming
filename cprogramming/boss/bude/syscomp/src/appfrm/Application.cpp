
#include "Application.h"

#include "ServiceMonitor.h"
#include "ErrorHandle.h"
#include "VersionInfo.h"

#include "log4cpp/Logger.h"

#include "base/Directory.h"
#include "base/SysParam.h"
#include "base/StringUtil.h"

#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <memory.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef WIN32_OS_CONFIG
#include <process.h>
#else
#include <unistd.h>
#endif

#include <fstream>
#include <strstream>
#include <iostream>


#ifndef GETOPT_COMPATIBLE
#define GETOPT_COMPATIBLE
#endif

#undef DUMP_ALERT_TO_MONITOR

//USING_NAMESPACE(log4cpp);

ErrorMessages g_errormessages;
ErrorMessages *const theErrorMessages = &g_errormessages;
log4cpp::Logger *theLogger = &(log4cpp::Logger::getLogger("OK"));
string theLoggerProcessID;


const char *SECT_SYSTEM = "\\SYSTEM";
const char *KEY_LOCKPATH = "SYS_LOCKPATH";
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
    PROCSTATE_RECOVERYFAILURE,
    PROCSTATE_BEFORELOOPFAILURE
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
    "Application INIT FAIL",
    "Application RECOVERYING",
    "Application RECOVERY FAIL",
    "Application_BEFORELOOP FAIL"
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
    "APPLICATION STARTUP -- initialization fail!",
    "APPLICATION STARTUP -- recovery complete!",
    "APPLICATION STARTUP -- recovery fail!",
    "APPLICATION STARTUP -- enter main loop!"
};

#define COMMAND_CONTINUE       'C'
#define COMMAND_STOP           'S'
#define COMMAND_PAUSE          'P'


volatile char Application::m_curcmd = COMMAND_CONTINUE;
// m_curcmd定义为volatile 直接从内存中读取，不从寄存器读取（优化可能会放在寄存器中）


Application::Application(): m_optind(1), m_sp(1), m_channelNo(-1),m_onlyone(false),
                            m_asdaemon(false), m_runinbkg(false), m_stop(0),
                            m_showVersion(false), m_theMonitor(0),m_fsortmethod(0)
{
    m_objectName = STATE_NOOBJECT;
    m_reserved = STATE_RESERVED[PROCSTATE_READY];
    m_status = PROC_READY;

    m_theErrorMessages = theErrorMessages;
    m_theErrorMessages->clean();

	m_version = new VersionInfo();
}


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

      case 'v':
          //show version information
          m_showVersion = true;
          break;

      case 'f':
          //support -f config_file_name
          m_configFile = optarg;
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

bool Application::initialization()
{
    m_configFile = "";
    if (!parseOpt())
        return false;

    if (m_showVersion) {
        showVersion();
        exit(0);
    }

    m_theErrorMessages->setApplicationInfo(m_name, m_channelNo);

    // init SysParam
    if ((m_theSysParam = new SysParam()) == 0)
    {
        m_theErrorMessages->insert(EAPPFRM_SYSINIT,"new SysParam");
        return false;
    }

    char *env;
    if (((env = getenv("WORK_HOME")) == 0) || (strcmp(env, "") == 0)) 
    {
        m_theErrorMessages->insert(EAPPFRM_SYSINIT,
          "Not define WORK_HOME!");
        env = "..";
        //sprintf(env, "..");
    }

    if ( m_configFile.empty()) 
    {
        m_configFile = env;
        m_configFile += "/etc/boss.conf";
    }

    m_theSysParam->initialize((char *)m_configFile.c_str());

    string section, value;
    section = SECT_SYSTEM;
    vector<string> paramVector;

    if (!m_theSysParam->getValue(section, KEY_LOCKPATH, paramVector))
    {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL, section +
                            SECTDELIM + KEY_LOCKPATH);
        m_lockpath = env;
        m_lockpath += "/bin/lock/";
    } else {
        m_lockpath = paramVector[0];
    }
    
    // 的到日志系统的DN描述
    if (!m_theSysParam->getValue(section, "LOGGER_DN", paramVector))
    {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL, section +
                            SECTDELIM + "LOGGER_DN");
        m_loggerDn = "";
    } else {
        m_loggerDn = paramVector[0];
    }
    
    // Logger 的配制信息
    map<string,string> loggerConfigureInfos;
    section = section + SECTDELIM + "LOGGER" ;
    if (!m_theSysParam->getValue(section,loggerConfigureInfos)){
        ;
    }

    m_logTypeId = -1;
    
	//m_argv[0] 有时应用程序名称前存在路径，argv_name[0]取应用程序名称
    char ** argv_name = m_argv;
    strcpy(argv_name[0],m_name); 

    m_loggerDn = m_loggerDn + "." + m_name + "." + StringUtil::toString(m_channelNo);  
    log4cpp::Logger::configure(loggerConfigureInfos);
    theLogger = &(log4cpp::Logger::getLogger(m_loggerDn));

    int argNum; //changed by gaojf 为了达到指定配置文件的目的    ,第一个参数必须为通道号
    if(m_argc < 2)
    {
        argNum = m_argc;
    }
    else
    {
        argNum = 2;
    }
    if((m_theMonitor = new ServiceMonitor(argNum, argv_name, m_lockpath)) == 0) {
    //if((m_theMonitor = new ServiceMonitor(m_argc, argv_name, m_lockpath)) == 0) {
      m_theErrorMessages->insert(EAPPFRM_SYSINIT, "new ServiceMonitor error");
      return false;
    }


    if(!m_theMonitor->initialize()) {
        m_theErrorMessages->insert(EAPPFRM_SYSINIT, 
        "ServiceMonitor initialize fail.");
        return false;
    }


    m_reserved = STATE_RESERVED[PROCSTATE_INITING];
    updateStatus();

    m_dir = new Directory();
    m_fsortmethod = Directory::SF_NONE;

    return true;
}


bool Application::recovery()
{
//    m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,
//                               "recovery ... (TO BE DETERMINED)");
    m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,
                               START_MSG[PROCSTATE_RECOVERYOK]);
    return true;
}


bool Application::end()
{
    m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,  "Application is end");
    writeAlert();

    m_objectName = "";
    m_reserved = "";
    m_status = PROC_EXIT;
    updateStatus();

    if(m_theMonitor)
        m_theMonitor->end();
    if(m_theSysParam)
        m_theSysParam->closeSection();

    delete m_theMonitor;
    delete m_theSysParam;

    return true;
}



bool Application::beforeLoop()
{
    return true;
}


bool Application::loopProcess()
{
    return true;
}


int Application::run()
{

    // application initialize
    if (!initialization()) {
        m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP, 
                START_MSG[START_INITFAIL]);
        writeAlert();

        m_reserved = STATE_RESERVED[PROCSTATE_INITINITFAILURE];
        updateStatus();
        return -1;
    }

    theLogger->alert(Grade::WARNING, E_SYSNOTIFY_STARTUP, START_MSG[START_INITOK]);

    writeAlert();

    m_status = PROC_READY;
    m_reserved = STATE_RESERVED[PROCSTATE_RECOVERYING];
    updateStatus();

    preLoop();

    if (! beforeLoop()) {
      writeAlert();
      m_status = PROC_RUN;
      m_reserved = STATE_RESERVED[PROCSTATE_BEFORELOOPFAILURE];
      updateStatus();
      return -1;            
    }

    if (!recovery()) {
      writeAlert();
      m_status = PROC_RUN;
      m_reserved = STATE_RESERVED[PROCSTATE_RECOVERYFAILURE];
      updateStatus();
      return -1;
    }

    mainLoop();

    end();

    return 0;
}



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




void Application::setoptstr(const char *optstring)
{
    assert(strlen(optstring) <= MAXAPPNAME);
    strcpy(m_optstring, optstring);
    strcat(m_optstring, "vf:");
}



int Application::getopt(char *opts)
{
    int c;
    char *cp;

    if (m_sp == 1)
    if ( (m_optind >= m_argc) || (m_argv[m_optind][0] != '-' ) ||
         (m_argv[m_optind][1] == '\0' ) )
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

        theLogger->alert(Grade::WARNING, errorInfo.no, errorInfo.message);
        
        if(m_theMonitor)
        {
//            if(m_theMonitor->alert(errorInfo.no, errorInfo.message))
//            {
//#ifdef DUMP_ALERT_TO_MONITOR
//                dumpstream << "WRITE ALERT ERROR, CHECH YOUR ALERT SYSTEM CONFIG!" << endl;
//#endif //DUMP_ALERT_TO_MONITOR
//            }
        }
        else
        {
            cout << "ALERT: " << errorNo << " - " << errorInfo.message << endl;
        }
    }
}



void Application::updateStatus()
{
    static log4cpp::NameValueList status;

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
    if( (strcmp(stat, status[0].value.c_str()) == 0 ) &&
        (m_objectName == status[1].value ) &&
        (m_reserved   == status[2].value ) )
    {
         return;
    }

    time_t l_time = time(NULL);
    tm *l_tm = localtime(&l_time);

    char buffer[32];
    sprintf(buffer, "%02d-%02d %02d:%02d:%02d", l_tm->tm_mon+1, l_tm->tm_mday,
            l_tm->tm_hour, l_tm->tm_min, l_tm->tm_sec);

    status[0].name = "应用状态";
    status[0].value = stat;
    status[1].name = "处理对象";
    status[1].value = m_objectName;
    status[2].name = "备注";
    status[2].value = m_reserved;
    status[3].name = "更新时间";
    status[3].value = buffer;

    // TODO for logger
    theLogger->state(status);
    
    //if(m_theMonitor)
    //    m_theMonitor->setStatus(status);
}


void Application::updateStatus(const char * status) 
{
    m_objectName = status;
    updateStatus();
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


bool Application::get_asdaemon() const
{
    return m_asdaemon;
}


void Application::set_asdaemon(bool left)
{
    m_asdaemon = left;
}


bool Application::get_onlyone() const
{
    return m_onlyone;
}

void Application::set_onlyone(bool left)
{
    m_onlyone = left;
}


bool Application::get_runinbkg() const
{
    return m_runinbkg;
}

void Application::set_runinbkg(bool left)
{
    m_runinbkg = left;
}


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
    signal(SIGSTOP, &Application::term_proc);
    signal(SIGCONT, &Application::term_proc);

    // get pid
#ifndef WIN32_OS_CONFIG
    m_pid = getpid();
#else
    m_pid = _getpid();
#endif


}



int Application::mainLoop()
{
    m_theErrorMessages->insert(E_SYSNOTIFY_STARTUP,START_MSG[PROCSTATE_RUNNING]);
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

            updateStatus();

            writeLog();

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
        // TODO logger
        string performId = theLogger->pBegin();
        theLogger->pEnd(performId,m_logstr.c_str());
        m_logstr = "";
    }
}

void Application::writeLog(int typeId, const string& str)
{
    m_logTypeId = typeId;
    m_logstr = str;
    writeLog();
}

void Application::writeLog(int typeId, const char* str)
{
    m_logTypeId = typeId;
    m_logstr = str;
    writeLog();
}


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
  
  theApp->onSignal(sig);
  
}

void Application::onSignal(const int sig) 
{
    return;
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




void Application::to_background()
{
#ifndef WIN32_OS_CONFIG
    // running as daemon
    // COMMENTS: using sun workshorp c++ 6 update 2 for sun os 5.8
    // close(0 or 1 or 2), then using fstream object as :
    // fstream in;
    // in.open         -OK    first time
    //    in.close     -OK    first time
    //    in.open      -FAILURE
    //    in.close
    /*
        close(0);
        close(1);
        close(2);
        setpgrp();
    */

    bool rc = true;
    setsid();
    int fd = open("/dev/null", O_RDWR);
    if(fd < 0)
    {
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
int Application::run_onlyone(const char *filename)
{
#ifndef WIN32_OS_CONFIG
    int    fd, val;
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
            exit(0);         // gracefully exit, daemon is already running
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



int Application::getFile(char *path, const int length)
{
    unsigned long size;
    return getFile(path, length, size);
}

int Application::getFile(char *path, const int length , unsigned long& filesize)
{
    int ret;
    if ((ret = m_dir->getFile(path, length, filesize)) != 0)
        return ret;
    m_dir->scanFiles((Directory::SortFlags)m_fsortmethod);
    return m_dir->getFile(path, length, filesize);
}

void Application::setFilter(const char *pattern)
{
    if (m_pattern != pattern)
    {
        m_pattern = pattern;
        m_dir->setFilter(pattern);
        m_dir->scanFiles((Directory::SortFlags)m_fsortmethod);
    }
}

void Application::setFileSortMethod(const int method)
{
    m_fsortmethod = method;
    m_dir->scanFiles((Directory::SortFlags)m_fsortmethod);
}

void Application::setMaxScan(const int num)
{
    m_dir->setMaxScan(num);
}

void Application::setPathIn(const char *path)
{
    m_dir->setPath(path);
    m_dir->scanFiles((Directory::SortFlags)m_fsortmethod);
}


void Application::setVersion(const VersionInfo & version)
{
	(*m_version) = version;
}


void Application::showVersion()
{
    cout << "==================================================" << endl;
    cout << (*m_version) << endl;
    cout << "==================================================" << endl;
}


int main(int argc, char *argv[])
{   
    theApp->setopt(argc, argv);
    return theApp->run();
}

