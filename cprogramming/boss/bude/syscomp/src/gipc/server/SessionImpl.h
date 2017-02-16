#ifndef __SESSION_IMPL_H__
#define __SESSION_IMPL_H__


#ifndef RIPC_NO_EXCEPTIONS
#define RIPC_THROW(x) throw x()
#define RIPC_TRY      try
#define RIPC_CATCH(x) catch(x)
#else
#define RIPC_THROW(x) abort()
#define RIPC_TRY      if (true)
#define RIPC_CATCH(x) else if (false)
#endif

#include "Session.h"
#include "HashMap.h"
#include "Socket.h"
#include "Thread.h"
#include "CriticalSection.h"

#include "LockImpl.h"
#include "MutexImpl.h"
#include "QueueImpl.h"
#include "BarrierImpl.h"
#include "SemaphoreImpl.h"
#include "SharedMemoryImpl.h"

#include "Request.h"
#include "Response.h"

#include "SessionEx.h"


class SessionImpl : public Session 
{ 
    friend class Server;
    friend class WaitObject;
  public:
    void run();
    void start(); 
    static void thread_proc runThread(void* arg);


    Semaphore* createSemaphore(char const* name, int initCount);
    Semaphore* openSemaphore(char const* name);
    Mutex* createMutex(char const* name, bool locked);
    Mutex* openMutex(char const* name);
    Queue* createQueue(char const* name);
    Queue* openQueue(char const* name);
    SharedMemory* createSharedMemory(char const* name, Object const& objDesc);
    SharedMemory* createSharedMemory(char const* name, void const* obj, size_t objSize);
    SharedMemory* openSharedMemory(char const* name);
    Lock* createLock(char const* name);
    Lock* openLock(char const* name);
    Barrier* createBarrier(char const* name, int nSessions);
    Barrier* openBarrier(char const* name);
    void close();
    void shutdownServer();
    void showServerInfo(char* buf, size_t bufSize);
    void getSessionID(char* buf, size_t bufSize);
    bool isLocal();
    bool isOk();
    void getErrorText(char* buf, size_t bufSize);

    SessionImpl(Server* server, Socket* socket = NULL);
    ~SessionImpl();

    void notify();
    void waitNotification(PrimitiveImpl* prim);
    void waitNotificationWithTimeout(PrimitiveImpl* prim, unsigned timeout); 
    void createEvent();
    void deleteEvent();

    void beginAccess(PrimitiveImpl* prim);
    void endAccess(PrimitiveImpl* prim);

    LockObject* addLock(PrimitiveImpl* prim, int flags = 0);
    void removeLock(LockObject* lock);

    void  readBody(void* buf, size_t size);
    void  skipBody(size_t size);

    char* dump(char* buf);

    SessionImpl*    next;
    SessionImpl*    prev;
    Thread          thread;
    CriticalSection cs;
    LockObject*     lockList;     // L1 list of locks
    LockObject*     freeLOBList;  // L1 list of free lock objects
    WaitObject*     waitFor;      // WOB list
    bool            running;
    bool            exists;
    Socket*         socket;
    Server*         server;
    unsigned        sequenceNo;
    HashMap         primitives;

#ifdef _WIN32
#else
    pthread_cond_t cond;
    int            signaled;
#endif
    SessionEx*     sessionex; 
};

#endif
