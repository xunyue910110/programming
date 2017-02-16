#ifndef __SERVER_H__
#define __SERVER_H__


#include "Config.h"
#include "Manager.h"


#include "Thread.h"
#include "util/Util.h" 
#include "util/cs_common.h"


#include <string>
#include <pthread.h>
#include <stdio.h>

class Socket;
class Listener;
class ParameterInterface;
class Session;

//#define MAXSIZE 1024

class Server : public Manager {
    friend class Session;
    friend class Listener;
  public:
    Server();
    virtual ~Server();

    virtual int  initialize( int debugLevel,const char * host, int port,
                    const char * config_file,const char * section);

    virtual int  setDebug(int debugLevel); 
    virtual int  update();
    virtual int  handleRequest(const string &reqs, string &result); 
    virtual int  handleRequest(const string &reqs, char *result); 
    virtual void stop();
    
    void run();


    //--------------------------------------------------------------------------
    // special functions for server managment
    void  startListener(const int &port);  
    void  stopListener(); 
	
 
    void  dialog(const bool & join_thread);
	  char *dump(char* buf);
    void  trace(char const* fmt, ...);

  protected:      
    void  startSession(Socket* s);
    int   checkSessions();
    void  stopSession(Session* session);
    void  stopAllSessions();

    vector<Session *>  m_sessions;
    Listener * m_listener;

    CriticalSection m_cs; 
    ParameterInterface * m_DbManager;
    
    int          m_listener_port;
    const char * m_config_file;
    const char * m_section;
    
    Thread m_thread;
    
};

#endif


