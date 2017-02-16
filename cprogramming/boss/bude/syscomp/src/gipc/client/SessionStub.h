#ifndef __SESSION_STUB_H__
#define __SESSION_STUB_H__

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

#include "Socket.h"
#include "CriticalSection.h"
#include "CommBuffer.h"

#include "Request.h"
#include "Response.h"

#include "LockStub.h"
#include "MutexStub.h"
#include "QueueStub.h"
#include "BarrierStub.h"
#include "SemaphoreStub.h"
#include "SharedMemoryStub.h"




class SessionStub : public Session { 
    friend class PrimitiveStub;
    friend class ClientFactory;
  public:

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

    ~SessionStub();

    void send(Request& req);
    void sendAndReceive(Request& req, Response& resp, Primitive* prim = NULL);
    void sendAndVerify(Request& req, Response& resp, Primitive* prim = NULL);
    bool sendAndCheckForOk(Request& req, Response& resp, Primitive* prim = NULL);

  protected:
    SessionStub(char const* host, int port);

    // 由该会话创建的原语链表
    PrimitiveStub*  primitives; // L2 null-terminated list of primitives created by this session
    Socket*         socket;
    CriticalSection cs;
    CommBuffer      buf;

};

#endif





