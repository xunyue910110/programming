#define RIPC_IMPLEMENTATION

#include <stdarg.h>
#include "PrimitiveImpl.h"
#include "Server.h"

Server Server::theServer;

char* Server::dump(char* buf) 
{
    RIPC_CS(cs);
    buf += sprintf(buf, "<<<-------- Sessions --------->>>\n");
    for (SessionImpl* s = sessions; s != NULL; s = s->next) { 
	buf = s->dump(buf);
    }
    buf += sprintf(buf, "<<<-------- Primitives --------->>>\n");
    for (PrimitiveImpl* prim = primitives; prim != NULL; prim = prim->next) { 
	buf = prim->dump(buf);
    }
    return buf;
}

inline void input(char const* prompt, char* buf, size_t buf_size)
{
    char* p;
    do {
	printf(prompt);
	*buf = '\0';
	fgets(buf, buf_size, stdin);
	p = buf + strlen(buf);
    } while (p <= buf+1);

    if (*(p-1) == '\n') {
	*--p = '\0';
    }
}

void Server::dialog() 
{ 
    char buf[32*1024];
    while (true) {	
	input("> ", buf, sizeof buf);
	if (strcmp(buf, "exit") == 0) { 
	    shutdown();	 
	    break;
	} else if (strcmp(buf, "info") == 0) { 
	    dump(buf);
	    fputs(buf, stdout);
	} else if (strcmp(buf, "help") == 0 || strcmp(buf, "?") == 0) { 
	    printf("Commands: exit, info, help\n");
	} else { 
	    printf("Invalid command %s\nCommands: exit, info, help\n", buf);
	}
    }
}

void thread_proc Server::acceptThread(void* arg)
{
    ((Server*)arg)->run();
}

void Server::start()
{
    running = true;
    thread.start(&acceptThread, this);
}

void Server::trace(char const* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void Server::run()
{
    while (running) { 
    	printf("OK startup!!\n");
	Socket* s = socket->accept();
	if (!running) { 
	    break;
	}
	if (s == NULL) { 
	    if (debug >= DEBUG_EXCEPTIONS) { 
		char errbuf[1024];
		socket->get_error_text(errbuf, sizeof errbuf);
		trace("Accept socket error: %s\n", errbuf); 
	    }
	} else { 
	    startSession(s);
	}
    }
}

void Server::shutdown()
{
    running = false;
    Socket* s = Socket::connect("localhost", port);
    delete s;
}


PrimitiveImpl* Server::getPrimitive(unsigned oid) {
    RIPC_CS(cs);
    PrimitiveImpl* prim = NULL; 
    if (oid > primTableUsed || (prim = primTable[oid]) == NULL) { 
	RIPC_THROW(NotFoundException);
    }
    return prim;
}


LockImpl* Server::getLock(unsigned oid)
{
    PrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != Primitive::IPC_LOCK) { 
	RIPC_THROW(NotFoundException);
    }
    return (LockImpl*)prim;
}

BarrierImpl* Server::getBarrier(unsigned oid)
{
    PrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != Primitive::IPC_BARRIER) { 
	RIPC_THROW(NotFoundException);
    }
    return (BarrierImpl*)prim;
}


SemaphoreImpl* Server::getSemaphore(unsigned oid)
{
    PrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != Primitive::IPC_SEMAPHORE) { 
	RIPC_THROW(NotFoundException);
    }
    return (SemaphoreImpl*)prim;
}


MutexImpl* Server::getMutex(unsigned oid)
{
    PrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != Primitive::IPC_MUTEX) { 
	RIPC_THROW(NotFoundException);
    }
    return (MutexImpl*)prim;
}


QueueImpl* Server::getQueue(unsigned oid)
{
    PrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != Primitive::IPC_QUEUE) { 
	RIPC_THROW(NotFoundException);
    }
    return (QueueImpl*)prim;
}


SharedMemoryImpl* Server::getSharedMemory(unsigned oid)
{
    PrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != Primitive::IPC_SHARED_MEMORY) { 
	RIPC_THROW(NotFoundException);
    }
    return (SharedMemoryImpl*)prim;
}


char* Server::strdup(char const* name) 
{
    char* copy = new char[strlen(name) + 1];
    strcpy(copy, name);
    return copy;
}



	

MutexImpl* Server::createMutex(SessionImpl* session, char const* name, 
				       bool locked, bool local) 
{
    RIPC_CS(cs);
    MutexImpl* mutex = (MutexImpl*)mutexes.get(name);
    if (mutex != NULL) { 
	session->exists = true;
    } else { 
	mutex = new MutexImpl(this, local ? strdup(name) : name, locked);
	mutexes.put(mutex);
	session->exists = false;
    }
    session->beginAccess(mutex);
    return mutex;
}    
	

SemaphoreImpl* Server::createSemaphore(SessionImpl* session, char const* name, 
					       int initCount, bool local)
{
    RIPC_CS(cs);
    SemaphoreImpl* semaphore = (SemaphoreImpl*)semaphores.get(name);
    if (semaphore != NULL) { 
	session->exists = true;
    } else { 
	semaphore = new SemaphoreImpl(this, local ? strdup(name) : name, initCount);
	semaphores.put(semaphore);
	session->exists = false;
    }
    session->beginAccess(semaphore);
    return semaphore;
}    
	
SharedMemoryImpl* Server::createSharedMemory(SessionImpl* session, char const* name, 
						     Object const& objDesc, bool copy)
{
    RIPC_CS(cs);
    SharedMemoryImpl* shmem = (SharedMemoryImpl*)shmems.get(name);
    if (shmem != NULL) { 
	session->exists = true;
    } else { 
	shmem = new SharedMemoryImpl(this, strdup(name), objDesc, copy);
	shmems.put(shmem);
	session->exists = false;
    }
    session->beginAccess(shmem);
    return shmem;
}    
	
SharedMemoryImpl* Server::createSharedMemory(SessionImpl* session, char const* name, 
						     size_t objSize)
{
    RIPC_CS(cs);
    SharedMemoryImpl* shmem = (SharedMemoryImpl*)shmems.get(name);
    if (shmem != NULL) { 
	session->skipBody(objSize);
	session->exists = true;
    } else { 
	shmem = new SharedMemoryImpl(this, name, session, objSize);
	shmems.put(shmem);
	session->exists = false;
    }
    session->beginAccess(shmem);
    return shmem;
}    
	
LockImpl* Server::createLock(SessionImpl* session, char const* name, bool local)
{
    RIPC_CS(cs);
    LockImpl* lock = (LockImpl*)locks.get(name);
    if (lock != NULL) { 
	session->exists = true;
    } else { 
	lock = new LockImpl(this, local ? strdup(name) : name);
	locks.put(lock);
	session->exists = false;
    }
    session->beginAccess(lock);
    return lock;
}    

QueueImpl* Server::createQueue(SessionImpl* session, char const* name, bool local)
{
    RIPC_CS(cs);
    trace("Session ID:%p %s %d",session,name,local);
    QueueImpl* queue = (QueueImpl*)queues.get(name);
    if (queue != NULL) { 
	session->exists = true;
    } else { 
	queue = new QueueImpl(this, local ? strdup(name) : name);
	queues.put(queue);
	session->exists = false;
    }
    session->beginAccess(queue);
    return queue;
}    

BarrierImpl* Server::createBarrier(SessionImpl* session, char const* name, int nSessions, bool local)
{
    RIPC_CS(cs);
    BarrierImpl* barrier = (BarrierImpl*)barriers.get(name);
    if (barrier != NULL) { 
	session->exists = true;
    } else { 
	barrier = new BarrierImpl(this, local ? strdup(name) : name, nSessions);
	barriers.put(barrier);
	session->exists = false;
    }
    session->beginAccess(barrier);
    return barrier;
}    


QueueImpl* Server::openQueue(SessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    QueueImpl* queue = (QueueImpl*)queues.get(name);
    if (queue != NULL) { 
	session->beginAccess(queue);
    }
    return queue;
}    

MutexImpl* Server::openMutex(SessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    MutexImpl* mutex = (MutexImpl*)mutexes.get(name);
    if (mutex != NULL) { 
	session->beginAccess(mutex);
    }
    return mutex;
}    

SharedMemoryImpl* Server::openSharedMemory(SessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    SharedMemoryImpl* shmem = (SharedMemoryImpl*)shmems.get(name);
    if (shmem != NULL) { 
	session->beginAccess(shmem);
    }
    return shmem;
}    

LockImpl* Server::openLock(SessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    LockImpl* lock = (LockImpl*)locks.get(name);
    if (lock != NULL) { 
	session->beginAccess(lock);
    }
    return lock;
}    

BarrierImpl* Server::openBarrier(SessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    BarrierImpl* barrier = (BarrierImpl*)barriers.get(name);
    if (barrier != NULL) { 
	session->beginAccess(barrier);
    }
    return barrier;
}    

SemaphoreImpl* Server::openSemaphore(SessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    SemaphoreImpl* semaphore = (SemaphoreImpl*)semaphores.get(name);
    if (semaphore != NULL) { 
	session->beginAccess(semaphore);
    }
    return semaphore;
}    


void Server::deleteLock(LockImpl* lock) 
{
    locks.remove(lock);
    deassignId(lock);
}

void Server::deleteSharedMemory(SharedMemoryImpl* shmem) 
{
    shmems.remove(shmem);
    deassignId(shmem);
}

void Server::deleteBarrier(BarrierImpl* barrier) 
{
    barriers.remove(barrier);
    deassignId(barrier);
}

void Server::deleteQueue(QueueImpl* queue) 
{
    queues.remove(queue);
    deassignId(queue);
}

void Server::deleteSemaphore(SemaphoreImpl* semaphore) 
{
    semaphores.remove(semaphore);
    deassignId(semaphore);
}

void Server::deleteMutex(MutexImpl* mutex) 
{
    mutexes.remove(mutex);
    deassignId(mutex);
}

void Server::assignId(PrimitiveImpl* prim)
{
    if (freePrimId == NULL) {  
	prim->id = ++primTableUsed;
	if (primTableUsed >= primTableSize) { 
	    PrimitiveImpl** newPrimTable = new PrimitiveImpl*[primTableUsed*2];
	    memcpy(newPrimTable, primTable, primTableSize*sizeof(PrimitiveImpl*));
	    primTableSize = primTableUsed*2;
	    delete[] primTable;
	    primTable = newPrimTable;
	}
    } else { 
	prim->id = freePrimId - primTable;
	freePrimId = (PrimitiveImpl**)*freePrimId;
    }
    primTable[prim->id] = prim;
    if (primitives != NULL) { 
	primitives->prev = prim;
    } 
    prim->next = primitives;
    prim->prev = NULL;
    primitives = prim;
}


void Server::deassignId(PrimitiveImpl* prim) 
{ 
    primTable[prim->id] = (PrimitiveImpl*)freePrimId;
    freePrimId = &primTable[prim->id];
    if (prim->next != NULL) { 
	prim->next->prev = prim->prev;
    }
    if (prim->prev != NULL) { 
	prim->prev->next = prim->next;
    } else {
	primitives = prim->next;
    }	   
}


static void charArrayDeallocator(Object const& objDesc) 
{ 
    delete[] (char*)objDesc.data;
}

bool Server::handleRequest(SessionImpl* session, Request& req, Response& resp)
{ 
    if (debug >= DEBUG_REQUESTS) { 
	PrimitiveImpl* prim = req.objectId != 0 ? getPrimitive(req.objectId) : NULL;
	trace("Session %p receive request %s:%s\n", 
	      session, req.getName(), (prim != NULL ? prim->name : req.name));
    }
    resp.statusCode = Response::OK;

    switch (req.opCode) { 
      case Request::WAIT:
	getPrimitive(req.objectId)->priorityWait(req.rank);
	break;
      case Request::TIMED_WAIT:
	if (!getPrimitive(req.objectId)->priorityWait(req.rank, req.value)) { 
	    resp.statusCode = Response::TIMEOUT_EXPIRED;
	}
	break;
      case Request::RESET:
	getPrimitive(req.objectId)->reset();
	break;

      case Request::SIGNAL_SEMAPHORE:
	getSemaphore(req.objectId)->signal(req.value);
	break;

      case Request::LOCK_MUTEX:
	getMutex(req.objectId)->priorityLock(req.rank);
	break;
      case Request::TIMED_LOCK_MUTEX:
	if (!getMutex(req.objectId)->priorityLock(req.rank, req.value)) { 
	    resp.statusCode = Response::TIMEOUT_EXPIRED;
	}
	break;
      case Request::UNLOCK_MUTEX:
	getMutex(req.objectId)->unlock();
	break;
      case Request::SHARED_LOCK:
	getLock(req.objectId)->prioritySharedLock(req.rank);
	break;
      case Request::TIMED_SHARED_LOCK:
	if (!getLock(req.objectId)->prioritySharedLock(req.rank, req.value)) { 
	    resp.statusCode = Response::TIMEOUT_EXPIRED;
	}
	break;
      case Request::EXCLUSIVE_LOCK:
	getLock(req.objectId)->priorityExclusiveLock(req.rank);
	break;
      case Request::TIMED_EXCLUSIVE_LOCK:
	if (!getLock(req.objectId)->priorityExclusiveLock(req.rank, req.value)) { 
	    resp.statusCode = Response::TIMEOUT_EXPIRED;
	}
	break;
      case Request::UNLOCK:
	getLock(req.objectId)->unlock();
	break;
      case Request::ENQUEUE:
	getQueue(req.objectId)->put(session, req.objDesc.size);
	break;
      case Request::BROADCAST:
	getQueue(req.objectId)->broadcast(session, req.objDesc.size);
	break;
      case Request::TIMED_DEQUEUE:
	getQueue(req.objectId)->priorityGet(req.rank, resp.objDesc, req.value);
	break;
      case Request::DEQUEUE:
	getQueue(req.objectId)->priorityGet(req.rank, resp.objDesc);
	break;
      case Request::GET_OBJECT:
	getSharedMemory(req.objectId)->get(resp.objDesc);
	break;
      case Request::SET_OBJECT:
	getSharedMemory(req.objectId)->set(session, req.objDesc.size);
	break;
      case Request::CREATE_SEMAPHORE:
	resp.objectId = createSemaphore(session, req.name, req.value, false)->id;
	if (session->exists) { 
	    resp.statusCode = Response::ALREADY_EXISTS;
	}
	break;
      case Request::CREATE_MUTEX:
	resp.objectId = createMutex(session, req.name, req.value != 0, false)->id;
	if (session->exists) { 
	    resp.statusCode = Response::ALREADY_EXISTS;
	}
	break;
      case Request::CREATE_QUEUE:
	resp.objectId = createQueue(session, req.name, false)->id;
	if (session->exists) { 
	    resp.statusCode = Response::ALREADY_EXISTS;
	}
	break;
      case Request::CREATE_SHMEM:
	resp.objectId = createSharedMemory(session, req.name, req.objDesc.size)->id;
	if (session->exists) { 
	    resp.statusCode = Response::ALREADY_EXISTS;
	}
	break;
      case Request::CREATE_LOCK:
	resp.objectId = createLock(session, req.name, false)->id;
	if (session->exists) { 
	    resp.statusCode = Response::ALREADY_EXISTS;
	}
	break;
      case Request::CREATE_BARRIER:
	resp.objectId = createBarrier(session, req.name, req.value, false)->id;
	if (session->exists) { 
	    resp.statusCode = Response::ALREADY_EXISTS;
	}
	break;
      case Request::OPEN_SEMAPHORE:
	{ 
	    SemaphoreImpl* semaphore = openSemaphore(session, req.name);
	    if (semaphore == NULL) {
		resp.statusCode = Response::NOT_FOUND;
	    } else { 
		resp.objectId = semaphore->id;
	    }
	}
	break;

      case Request::OPEN_MUTEX:
	{ 
	    MutexImpl* mutex = openMutex(session, req.name);
	    if (mutex == NULL) {
		resp.statusCode = Response::NOT_FOUND;
	    } else { 
		resp.objectId = mutex->id;
	    }
	}
	break;
      case Request::OPEN_QUEUE:
	{ 
	    QueueImpl* queue = openQueue(session, req.name);
	    if (queue == NULL) {
		resp.statusCode = Response::NOT_FOUND;
	    } else { 
		resp.objectId = queue->id;
	    }
	}
	break;
      case Request::OPEN_SHMEM:
	{ 
	    SharedMemoryImpl* shmem = openSharedMemory(session, req.name);
	    if (shmem == NULL) {
		resp.statusCode = Response::NOT_FOUND;
	    } else { 
		resp.objectId = shmem->id;
	    }
	}
	break;
      case Request::OPEN_LOCK:
	{ 
	    LockImpl* lock = openLock(session, req.name);
	    if (lock == NULL) {
		resp.statusCode = Response::NOT_FOUND;
	    } else { 
		resp.objectId = lock->id;
	    }
	}
	break;
      case Request::OPEN_BARRIER:
	{ 
	    BarrierImpl* barrier = openBarrier(session, req.name);
	    if (barrier == NULL) {
		resp.statusCode = Response::NOT_FOUND;
	    } else { 
		resp.objectId = barrier->id;
	    }
	}
	break;
      case Request::CLOSE_PRIMITIVE:
	session->endAccess(getPrimitive(req.objectId)); 
	break;
      case Request::CLOSE_SESSION:		    
	return false;
      case Request::SHOW_INFO:
	{ 
	    char* buf = new char[RIPC_INFO_BUF_SIZE];
	    char* eof = dump(buf);
	    resp.objDesc.data = buf;
	    resp.objDesc.size = eof - buf + 1;
	    resp.objDesc.deallocator = &charArrayDeallocator;
	}
	break;
      case Request::SHUTDOWN:
	shutdown();
	return false;
      case Request::SESSION_ID:
        {
           char* buf = new char[30];
           sprintf(buf,"%p",session);
           resp.objDesc.data=buf;
           resp.objDesc.size=30;
           resp.objDesc.deallocator = &charArrayDeallocator;
        }
	break;
      default:
	resp.statusCode = Response::INTERNAL_ERROR;		
    }
    if (debug >= DEBUG_REQUESTS) { 
	trace("Session %p comlete request %s\n", session, req.opCodeNames[req.opCode]);
    }
    return true;
}

SessionImpl* Server::getSession()
{
    SessionImpl* session = (SessionImpl*)CurrentThread::getThreadData();
    if (session == NULL) { 
	session = new SessionImpl(this);
	CurrentThread::setThreadData(session);
	cs.enter();
	session->next = sessions;
	if (sessions != NULL) { 
	    sessions->prev = session;
	}
	session->prev = NULL;
	sessions = session;	
	cs.leave();
	onSessionCreate(session);
    }
    return session;
}

void Server::startSession(Socket* s) 
{ 
    SessionImpl* session = new SessionImpl(this, s);
    if (debug >= DEBUG_SESSIONS) {
	trace("Start session %p\n", session);
    }
    cs.enter();
    session->next = sessions;
    if (sessions != NULL) { 
	sessions->prev = session;
    }
    session->prev = NULL;
    sessions = session;	
    cs.leave();

    onSessionCreate(session);
    session->start(); //启动新的线程处理连接
    onSessionRun(session);
}


void Server::stopSession(SessionImpl* session) 
{
    if (debug >= DEBUG_SESSIONS) {
	trace("Stop session %p\n", session);
    }
    cs.enter();
    if (session->prev == NULL) { 
	sessions = session->next;
    } else { 
	session->prev->next = session->next;
    }
    if (session->next != NULL) { 
	session->next->prev = session->prev;
    }
    cs.leave();
    onSessionClose(session);
}

void Server::waitNotification(WaitObject& wob, PrimitiveImpl* prim) 
{ 
    SessionImpl* session = getSession();
    wob.session = session;
    prim->addWaitObject(&wob);
    session->waitFor = &wob;
    if ((wob.flags & LOCKED_SERVER) != 0) { 
	cs.leave();
    }
    session->waitNotification(prim);
}

void Server::waitNotificationWithTimeout(WaitObject& wob, PrimitiveImpl* prim, unsigned timeout) 
{ 
    if (timeout != 0){ 
	SessionImpl* session = getSession();
	cs.enter();
	wob.session = session;
        prim->addWaitObject(&wob);
	session->waitFor = &wob;
	wob.flags |= TIMED_WAIT;
	cs.leave();
	session->waitNotificationWithTimeout(prim, timeout);
	if (!wob.signaled) { 
	    cs.enter();
	    wob.session->waitFor = NULL;
            wob.unlink();
	    cs.leave();
	}
    }
}

Server::~Server() {}
