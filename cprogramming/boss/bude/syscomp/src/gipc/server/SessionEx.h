#ifndef __SessionEx_H__
#define __SessionEx_H__ 

#include "Thread.h"
#include "CriticalSection.h"
#include "Session.h"
#include "Request.h"


class SessionEx
{
    friend class Server;
    friend class SessionImpl;
  public:

    void start();
    static void thread_proc runThread(void*);
    void run();
    SessionEx(Server* ,SessionImpl* );
    virtual ~SessionEx();

  //potect:
    virtual bool initialize();
    virtual bool recovery();
    virtual bool end();
    virtual bool handleRequest(CommPacket& ); 
    virtual int  getRequest(CommPacket& req);
    virtual void onClientTimeout();

    char* dump(char* buf);
    Session* getSession();
    virtual void close();

    Thread          thread;
    bool            running;
    bool            auto_running;
    int             stop;

    Server*         server;
    SessionImpl*    session;
    SessionImpl*    theSession;
    CriticalSection cs;

    Queue*          req_queue;
    Queue*          resp_queue;
    char            client_addr[50];
  private:
    void preLoop();
    int  mainLoop();
};


#endif //__SessionEx_H__ 
