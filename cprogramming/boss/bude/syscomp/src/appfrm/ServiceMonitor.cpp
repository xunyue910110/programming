
#include "ServiceMonitor.h"
#include "FPush.h"
#include "UdpSender.h"

#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 5556
#define CMD_MAX 256
#define ALARM_BUFFER_SIZE 1024
#define LOG_BUFFER_SIZE 1024
#define STATUS_BUFFER_SIZE 1024


class InternalServiceMonitor
{
public:
    InternalServiceMonitor(int argc, char *argv[], const string& workdir);
    InternalServiceMonitor();
    ~InternalServiceMonitor();

    bool initialize();
    bool end();

    bool debug(int code , const char * message);
    bool info(int code , const char * messgage);

    bool alert(int code, const char * message);
    bool log(int typeId, const char * data);

    bool setStatus(const NameValueList & status);

    void setDump(ostream *o)
      { m_os =(o); };
      
    void traceLevel(int level)
      { m_traceLevel = level;}; 
      
    bool isDebugEnabled() 
      { return (m_traceLevel>= MONITOR_TRACE_DEBUG);  };\

    bool isInfoEnabled() 
      { return (m_traceLevel>= MONITOR_TRACE_INFO);  };   

private:
    bool lock();
    bool unLock();
    
private:
    int  m_lock;
    int  m_argc;
    char * const * m_argv;
    char m_commandLine[CMD_MAX];
    string m_workdir;
    
    //FPush m_debugs;
    //FPush m_infos;
    FPush m_alarms;

    FPush m_logs;
    UdpSender m_status;

    int m_traceLevel;
    ostream * m_os;
};

InternalServiceMonitor::InternalServiceMonitor(int argc, char *argv[],
        const string& workdir) : m_status(), 
                                 m_alarms(ALARM_BUFFER_SIZE), 
                                 m_logs(LOG_BUFFER_SIZE),
                                 m_os(0)
{
    m_argc = argc;
    m_argv = argv;
    m_workdir = workdir;

    if(m_workdir.length() && m_workdir[m_workdir.length() -1 ] != '/') {
        m_workdir += '/';
    }

    m_commandLine[0] = '\0';
    for(int i = 0; i < argc 
        && strlen(m_commandLine) + strlen(argv[i]) + 1 < CMD_MAX; i++)
    {
        if(i != 0 )
        {
            strcat(m_commandLine, " ");
        }
        strcat(m_commandLine, argv[i]);
    }
}

InternalServiceMonitor::InternalServiceMonitor()
{ 
	m_os = 0;
}

InternalServiceMonitor::~InternalServiceMonitor()
{
}

bool InternalServiceMonitor::initialize()
{
    string sdir = m_workdir + m_commandLine;
    if(access(sdir.c_str(), F_OK))
    {
        if(mkdir(sdir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR 
                             | S_IRGRP | S_IROTH | S_IXOTH))
        {
            return false;
        }
    }

    if(!lock()) {
        return false;
    }

    try 
    {
      m_alarms.open((sdir + "/alarms").c_str());
      m_logs.open((sdir + "/logs").c_str());
      m_status.open((char *)"localhost", PORT);
    }
    catch (const FPushException& e)
    {
        return false;
    }

    return true;
}


bool InternalServiceMonitor::end()
{
    unLock();
    m_alarms.close();
    m_logs.close();
    m_status.close();

    return true;
}


bool InternalServiceMonitor::lock()
{
    int  fd, val;
    char buf[10];

    if ((m_lock = open((m_workdir + m_commandLine + "/lock").c_str(), 
                        O_WRONLY | O_CREAT,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
    {
        return false;
    }
    // try and set a write lock on the entire file
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    return fcntl(m_lock, F_SETLK, &lock) == 0;
}


bool InternalServiceMonitor::unLock()
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(m_lock, F_SETLK, &lock);

    close(m_lock);
    unlink((m_workdir + m_commandLine + "/lock").c_str());

    return true;
}


bool InternalServiceMonitor::debug(int code, const char * message)
{
    try
    {
    	  if (m_traceLevel < MONITOR_TRACE_DEBUG)
    	    return true;

        if (m_os!=0) 
          {(*m_os) << "[DEBUG]:"<<code<<":"<<message<<endl;}
    	    
        char buffer[ALARM_BUFFER_SIZE + 1];
        int len = sprintf(buffer, "%d@%d@", time(NULL), code);
        strncpy(buffer + len, message, ALARM_BUFFER_SIZE - len);
        buffer[ALARM_BUFFER_SIZE] = '\0';
        m_alarms.push(buffer);
    } 
    catch (const FPushException& e) 
    {
        return false;
    }
    
    return true;
}

bool InternalServiceMonitor::info(int code, const char * message)
{
    try
    {
    	  if (m_traceLevel < MONITOR_TRACE_INFO)
    	    return true;

        if (m_os!=0) 
          {(*m_os) << "[INFO ]:"<<code<<":"<<message<<endl;}

        char buffer[ALARM_BUFFER_SIZE + 1];
        int len = sprintf(buffer, "%d@%d@", time(NULL), code);
        strncpy(buffer + len, message, ALARM_BUFFER_SIZE - len);
        buffer[ALARM_BUFFER_SIZE] = '\0';

        m_alarms.push(buffer);
    } 
    catch (const FPushException& e) 
    {
        return false;
    }
    
    return true;
}

bool InternalServiceMonitor::alert(int code, const char * message)
{
    try
    {
    	  if (m_traceLevel < MONITOR_TRACE_ALERT)
    	    return true;
        
        if (m_os!=0) 
          {(*m_os) << "[ALERT]:"<<code<<":"<<message<< endl;}

        char buffer[ALARM_BUFFER_SIZE + 1];
        int len = sprintf(buffer, "%d@%d@", time(NULL), code);
        strncpy(buffer + len, message, ALARM_BUFFER_SIZE - len);
        buffer[ALARM_BUFFER_SIZE] = '\0';

        m_alarms.push(buffer);
    } 
    catch (const FPushException& e) 
    {
        return false;
    }
    return true;
}

bool InternalServiceMonitor::log(int type, const char * data)
{
    try
    {
        char buffer[LOG_BUFFER_SIZE + 1];

        int len = sprintf(buffer, "%d@%d@", time(NULL), type);
        strncpy(buffer + len, data, LOG_BUFFER_SIZE - len);
        buffer[LOG_BUFFER_SIZE] = '\0';

        info(MONITOR_TRACE_INFO,(string("[LOGER]:") + buffer).c_str());

        m_logs.push(buffer);

    } 
    catch (const FPushException& e) 
    {
        return false;
    }
    
    return true;
}


bool InternalServiceMonitor::setStatus(const NameValueList & state)
{
    try
    {
        int i, len = 0;
        char buffer[STATUS_BUFFER_SIZE + 1];

        //检查空间
        for(i=0; i<state.size(); i++)
        {
            len += state[i].m_name.length() + state[i].m_value.length();
        }

        if(len + state.size() * 2 > STATUS_BUFFER_SIZE)
        {
            return false;
        }

        //串化
        buffer[0] = '\0';
        strcat(buffer, m_commandLine);
        for(i = 0; i < state.size(); i++)
        {
            strcat(buffer, "@");
            strcat(buffer, state[i].m_name.c_str());
            strcat(buffer, "@");
            strcat(buffer, state[i].m_value.c_str());
        }

        info(MONITOR_TRACE_INFO,(string("[STATE]:") + buffer).c_str());

        //写出
        m_status.send(buffer, strlen(buffer));
    }
    catch(const UdpSenderException& e)
    {
        return false;
    }

    return true;
}

ServiceMonitor::ServiceMonitor(int argc, char* argv[], const string & workdir)
{
    m_monitor = new InternalServiceMonitor(argc, argv, workdir);
}

ServiceMonitor::ServiceMonitor()
{
    m_monitor = new InternalServiceMonitor();
}

ServiceMonitor::~ServiceMonitor()
{
    delete m_monitor;
}


bool ServiceMonitor::initialize()
  { return m_monitor->initialize(); }

bool ServiceMonitor::end()
  { return m_monitor->end(); }

bool ServiceMonitor::info(int code , const char * message) 
  { return m_monitor->info(code,message); }

bool ServiceMonitor::debug(int code , const char * message) 
  { return m_monitor->debug(code,message); }

bool ServiceMonitor::alert(int code, const char * message)
  { return m_monitor->alert(code, message); }

bool ServiceMonitor::log(int typeId, const char * data)
  { return m_monitor->log(typeId, data); }

bool ServiceMonitor::setStatus(const NameValueList & status)
  { return m_monitor->setStatus(status); }

void ServiceMonitor::traceLevel(int level)
  { m_monitor->traceLevel(level);};

bool ServiceMonitor::isDebugEnabled() 
  {return m_monitor->isDebugEnabled();}

bool ServiceMonitor::isInfoEnabled() 
  {return m_monitor->isInfoEnabled();}

void ServiceMonitor::setDump(ostream *o)
  { m_monitor->setDump(o);};



//*//--------------------------------------------------------------/
//*//-test for the class-/

//int main() 
//{
//	ServiceMonitor monitor;
//	cout << "start up ... \n";	
//	monitor.traceLevel(MONITOR_TRACE_INFO);
//	monitor.setDump(&cout);
//
//	monitor.debug(1,string("ok").c_str()); 
//
//  MONITOR_INFO((&monitor),"this a nather"); 
//	MONITOR_DEBUG((&monitor),"this is a debug");
//	monitor.info(1,"this is a book");
//	monitor.log(89,"ok@ok@ok");
//	monitor.alert(89,"alert the book");
//	MONITOR_ALERT((&monitor),"ALERT OK");
//	NameValue o;
//	o.m_name = "rate -c1";
//	o.m_value= "FILE1";
//	NameValueList states;
//	states.push_back(o);
//
//	monitor.setStatus(states); 
//
//	return 0;
//}


