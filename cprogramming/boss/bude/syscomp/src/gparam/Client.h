#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>
#include <pthread.h>

#include "Manager.h"
#include <string>

class Socket;

class Client : public Manager {
  public:
    virtual int initialize(int debugLevel,const char * host, int port,
                        const char * config_file,const char * section);

    virtual int connect(const char * host, int port);
    virtual int disconnect();
    virtual bool isConnected();
    
    virtual void  stop();
    
    virtual int notify();//I am here
    virtual int handleRequest(const string &reqs, string &result); 
    virtual int handleRequest(const string &reqs, char *result); 

    Client();
    virtual ~Client();
    
  private:
    const char*  m_host;
    int          m_port;
    Socket*      m_comm; 
    pthread_mutex_t m_clientMutex;    // 互斥量  	
    
};


#endif

