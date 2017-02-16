#ifndef __SERVER_H__
#define __SERVER_H__

#include "Factory.h"
#include "HashMap.h"
#include "SessionImpl.h"

#define RIPC_INFO_BUF_SIZE (32*1024)


class Server {
    friend class SessionImpl;
    friend class PrimitiveImpl;
  public:
    /**
     * Get instance of the server 
     */
    static Server theServer; //为工厂类服务

    virtual Session* create(char const*, int) { 
	return getSession();
    }
    
    /**
     * Dump information about state of the server:
     * all active sessions and all primitives opened by these sessions
     * @param out output stream
     */
    virtual char* dump(char* buf);

    /**
     * Server interactive dialog
     */
    virtual void dialog();

    
    void initialize() { 
	primTableSize = primTableUsed = 0;
	primTable = NULL;
	primitives = NULL;
	sessions = NULL;
	freePrimId = NULL;
    }

    /**
     * Constructor server for remote connections. 
     * This constructor creates server socket at which client connections
     * will be accepted
     * @param port port number at which client connections will be accepted
     */
    Server(int port, int debugLevel) { 
	initialize();
	socket = Socket::create(NULL, port);
	this->port = port;
	debug = debugLevel;
    }

    Server() {
	initialize();
    }

    virtual ~Server();
    
    virtual void trace(char const* fmt, ...);


    // 被线程调用 有客户端连接
    static void thread_proc acceptThread(void*);
    
    // 启动socket监听 创建新的会话对象处理连接
    virtual void run(); 

    // 开始一个新的线程 启动socket监听
    virtual void start(); 


    // 停止服务
    void shutdown();

    enum WOB_GENERIC_FLAGS { 
	LOCKED_SERVER = 0x1000,
	TIMED_WAIT    = 0x2000
    };

    virtual void deleteSemaphore(SemaphoreImpl* event);
    virtual void deleteMutex(MutexImpl* event);
    virtual void deleteQueue(QueueImpl* event);
    virtual void deleteSharedMemory(SharedMemoryImpl* event);
    virtual void deleteBarrier(BarrierImpl* event);
    virtual void deleteLock(LockImpl* event);

    virtual void assignId(PrimitiveImpl* prim);
    virtual void deassignId(PrimitiveImpl* prim); 

    virtual void waitNotification(WaitObject& wob, PrimitiveImpl* prim);

    virtual void waitNotificationWithTimeout(WaitObject& wob, PrimitiveImpl* prim, unsigned timeout);

    virtual SessionImpl* getSession();

    /**
     * Debug levels
     */
    enum DEBUG_LEVELS { 
	DEBUG_NONE, 
	DEBUG_EXCEPTIONS, 
	DEBUG_SESSIONS, 
	DEBUG_REQUESTS, 

	DEFAULT_DEBUG_LEVEL = DEBUG_EXCEPTIONS

    };
    
    inline void lock() { 
	cs.enter();
    }
    
    inline void unlock() { 
	cs.leave();
    }
    
  protected:
    virtual PrimitiveImpl*    getPrimitive(unsigned oid);

    virtual MutexImpl*        getMutex(unsigned oid);
    virtual QueueImpl*        getQueue(unsigned oid);
    virtual SemaphoreImpl*    getSemaphore(unsigned oid);
    virtual LockImpl*         getLock(unsigned oid);
    virtual SharedMemoryImpl* getSharedMemory(unsigned oid);
    virtual BarrierImpl*      getBarrier(unsigned oid);

    virtual MutexImpl*        createMutex(SessionImpl* session, char const* name, 
					      bool locked, bool local);
    virtual SemaphoreImpl*    createSemaphore(SessionImpl* session, char const* name, 
					       int initCount, bool local);
    virtual SharedMemoryImpl* createSharedMemory(SessionImpl* session, char const* name, 
						     Object const& objDesc, bool copy);
    virtual SharedMemoryImpl* createSharedMemory(SessionImpl* session, char const* name, 
						     size_t objSize);
    virtual QueueImpl*        createQueue(SessionImpl* session, char const* name, bool local);
    virtual BarrierImpl*      createBarrier(SessionImpl* session, char const* name, 
						int nSessions, bool local);
    virtual LockImpl*         createLock(SessionImpl* session, char const* name, bool local);

    virtual MutexImpl*        openMutex(SessionImpl* session, char const* name);
    virtual SemaphoreImpl*    openSemaphore(SessionImpl* session, char const* name);
    virtual LockImpl*         openLock(SessionImpl* session, char const* name);
    virtual QueueImpl*        openQueue(SessionImpl* session, char const* name);
    virtual BarrierImpl*      openBarrier(SessionImpl* session, char const* name);
    virtual SharedMemoryImpl* openSharedMemory(SessionImpl* session, char const* name);


    virtual bool handleRequest(SessionImpl* session, Request& req, Response& resp);
    virtual void startSession(Socket* s);
    virtual void stopSession(SessionImpl* session);

    virtual void onSessionCreate(SessionImpl* session){};//service not running
    virtual void onSessionRun(SessionImpl* session){};//
    virtual void onSessionClose(SessionImpl* session){};//

    virtual char* strdup(char const* name);

    bool            running;  //服务状态
    int             port;   
    int             debug;
    Socket*         socket;
    Thread          thread;

    HashMap         semaphores;
    HashMap         barriers;
    HashMap         mutexes;
    HashMap         events;
    HashMap         queues;
    HashMap         shmems;
    HashMap         locks;

    SessionImpl*    sessions;   // L2 null terminated list of sessions
    PrimitiveImpl*  primitives; // L2 null terminated list of sessions
    PrimitiveImpl** primTable;  // array of pointer to primitives with objectId used as index
    PrimitiveImpl** freePrimId; // pointer to L1 list of free elements of primTable
    unsigned        primTableSize;
    unsigned        primTableUsed;
    CriticalSection cs;//代码临界区
};

#endif

