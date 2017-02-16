#ifndef __SESSION_IMPL_H__
#define __SESSION_IMPL_H__

#include "Config.h"


#include "Socket.h"
#include "Thread.h"

#include <string>
#include <time.h>


class Server;

class Session {
  public:
    static void thread_proc runThread(void* arg);
    void start(Socket* comm = NULL); 
    void run();
    void stop();

    int  handleRequest(const string &reqs, string &result);

    int  handleRequest(const string &reqs, char *result);

    Session(Server* server);
    ~Session();
	
    char* dump(char* buf);
    bool  isOk();
    void  getErrorText(char* buf, size_t bufSize);

	  bool  m_running;
	int getBufferSize();

  protected:
    void  reset();
    void  trace(char const* fmt, ...);

    Thread  thread;
	time_t  m_last_active;
	string  m_ID;
	string  m_peer_name;
    string   m_clientID;
    
    char *m_buffer;
    
    int m_bufferSize;
    
    Socket* m_comm;
    Server* m_server;

	  CriticalSection cs;

};


#endif

