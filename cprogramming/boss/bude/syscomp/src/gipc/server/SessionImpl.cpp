#define RIPC_IMPLEMENTATION

#include "SessionImpl.h"
#include "Server.h"
#include "CommBuffer.h"
#include "IoException.h"
#include "LockObject.h"
#include "WaitObject.h"

#include "errno.h"

void SessionImpl::notify() 
{ 
    signaled = true;
    waitFor = NULL;
    pthread_cond_signal(&cond);
}

void SessionImpl::waitNotification(PrimitiveImpl* prim) 
{ 
    signaled = false;
    do { 
	int rc = pthread_cond_wait(&cond, &prim->cs.mutex);
	if (rc != 0 && rc != EINTR) { 
	    RIPC_THROW(InterruptedException);
	}
    } while (!signaled); 
}

void SessionImpl::waitNotificationWithTimeout(PrimitiveImpl* prim, unsigned timeout) 
{ 
    struct timeval cur_tv;
    gettimeofday(&cur_tv, NULL);
    struct timespec abs_ts; 
    unsigned usec = (cur_tv.tv_usec + timeout%1000*1000);
    
    signaled = false;
    do { 
	abs_ts.tv_nsec = usec % 1000000 * 1000;
	abs_ts.tv_sec = cur_tv.tv_sec + timeout/1000 + usec / 1000000; 
	int rc = pthread_cond_timedwait(&cond, &prim->cs.mutex, &abs_ts);
	if (rc == ETIMEDOUT) { 
	    break;
	}
	if (rc != 0 && rc != EINTR) { 
	    RIPC_THROW(InterruptedException);
	}		
    } while (!signaled); 
}

void SessionImpl::createEvent() 
{ 
    pthread_cond_init(&cond, NULL);
    signaled = false;
}

void SessionImpl::deleteEvent() 
{ 
    pthread_cond_destroy(&cond);
}

SessionImpl::SessionImpl(Server* server, Socket* socket)
{
    this->server = server;
    this->socket = socket;
    sequenceNo = 0;
    running = true;
    waitFor = NULL;
    freeLOBList = NULL;
    lockList = NULL;
    exists = false;
    createEvent();
    sessionex=NULL;
}

SessionImpl::~SessionImpl()
{
    close();
}

void thread_proc SessionImpl::runThread(void* arg) 
{
    ((SessionImpl*)arg)->run();
}

void SessionImpl::start()
{
    running = true;
    thread.start(&runThread, this);
    if (sessionex!=NULL && sessionex->auto_running)
    sessionex->start();
}

void SessionImpl::run() 
{ 
    char errmsg[256];
    CommBuffer buf;
    CurrentThread::setThreadData(this);

    while (running) {
	Request  req;
	Response resp;
	RIPC_TRY { 
	    if (socket->read(buf.getAddr(), sizeof(RequestHeader))) { 
		buf.unpackRequest(req);
		if (req.nameLen != 0) { 
		    req.name = new char[req.nameLen];
		    if (!socket->read(req.name, req.nameLen)) { 
			socket->get_error_text(errmsg, sizeof(errmsg));
			server->trace("Failed to read primitive name: %p: %s\n", this, errmsg);	    
			break;
		    }
		}
		sequenceNo = req.sequenceNo;
		if (!server->handleRequest(this, req, resp)) { 
		    break;
		}
	    } else { 
		socket->get_error_text(errmsg, sizeof(errmsg));
		server->trace("Failed to read request for session %p: %s\n", this, errmsg);	    
		break;
	    }	
	} RIPC_CATCH(IoException const&) { 
	    socket->get_error_text(errmsg, sizeof(errmsg));
	    server->trace("Socket IO error: %s\n", this, errmsg);	    
	    break;
	} RIPC_CATCH(DeadlockException const&) { 
	    resp.statusCode = Response::DEADLOCK;
	} RIPC_CATCH(InternalException const&) { 
	    resp.statusCode = Response::INTERNAL_ERROR;
	} RIPC_CATCH(InvalidParameterException const&) { 
	    resp.statusCode = Response::INVALID_PARAMETER;
	} RIPC_CATCH(NotFoundException const&) { 
	    resp.statusCode = Response::NOT_FOUND;
	} RIPC_CATCH(NotOwnerException const&) { 
	    resp.statusCode = Response::NOT_OWNER;
	} RIPC_CATCH(InterruptedException const&) { 
	    resp.statusCode = Response::INTERRUPTED;
	}
	resp.sequenceNo = sequenceNo;
	buf.packResponse(resp);
	if (!socket->write(buf.getAddr(), buf.getSize())) { 			
	    socket->get_error_text(errmsg, sizeof(errmsg));
	    server->trace("Failed to send response for session %p: %s\n", this, errmsg);	    
	    break;
	}
    }
    delete this;
}

void SessionImpl::close()
{
    RIPC_CS(cs);
    deleteEvent();
    LockObject *nextLOB, *lob;
    PrimitiveImpl *prim;
    for (lob = lockList; lob != NULL; lob = nextLOB) { 
	nextLOB = lob->nextLock;
	lob->unlock();
    }    
    lockList = NULL;
    for (lob = freeLOBList; lob != NULL; lob = nextLOB) { 
	nextLOB = lob->nextLock;
	delete lob;
    }
    freeLOBList = NULL;
    HashMap::Iterator iterator(primitives);    
    while ((prim = iterator.next()) != NULL) { 
	prim->endAccess(this);
    }
    running = false;
    delete socket;
    
    if (server != NULL) { 
	server->stopSession(this);
	server = NULL;
    }
}


void SessionImpl::beginAccess(PrimitiveImpl* prim)
{
    RIPC_CS(cs);
    prim->beginAccess(this);
    primitives.put(prim);
}
    
void SessionImpl::endAccess(PrimitiveImpl* prim)
{
    RIPC_CS(cs);
    primitives.remove(prim);
    prim->endAccess(this);
}

LockObject* SessionImpl::addLock(PrimitiveImpl* prim, int flags) 
{
    RIPC_CS(cs);
    LockObject* lob = freeLOBList;
    if (lob != NULL) { 
	freeLOBList = lob->nextLock;
    } else { 
	lob = new LockObject(this);
    }
    lob->prim = prim;
    lob->flags = flags;
    lob->nextLock = lockList;
    lockList = lob;
    return lob;
}

void SessionImpl::removeLock(LockObject* lock) 
{ 
    RIPC_CS(cs);
    LockObject **lpp;
    for (lpp = &lockList; *lpp != lock; lpp = &(*lpp)->nextLock);
    *lpp = lock->nextLock;
    lock->nextLock = freeLOBList;
    freeLOBList = lock;
}

char* SessionImpl::dump(char* buf)
{
    RIPC_CS(cs);
    buf += sprintf(buf, "Session %p\n", this);
    if (waitFor != NULL) { 
	 buf += sprintf(buf, "  Waiting for: %s:%s\n", waitFor->prim->getTypeName(), waitFor->prim->name);
    }
    if (lockList != NULL) { 
	buf += sprintf(buf, "  Locking:\n");
	for (LockObject* lob = lockList; lob != NULL; lob = lob->nextLock) { 
	    buf += sprintf(buf, "    %s:%s\n", lob->prim->getTypeName(), lob->prim->name);
	}
    }
    buf += sprintf(buf, "  Opened Primitives:\n");
    HashMap::Iterator iterator(primitives);    
    PrimitiveImpl* prim;
    while ((prim = iterator.next()) != NULL) { 
	buf += sprintf(buf, "    %s:%s\n", prim->getTypeName(), prim->name);
    }
    return buf;
}


//------------------------------------------------------------------------------
//
Semaphore* SessionImpl::createSemaphore(char const* name, int initCount)
{
    return server->createSemaphore(this, name, initCount, true);
}

Semaphore* SessionImpl::openSemaphore(char const* name)
{
    return server->openSemaphore(this, name);
}

Mutex* SessionImpl::createMutex(char const* name, bool locked)
{
    return server->createMutex(this, name, locked, true);
}

Mutex* SessionImpl::openMutex(char const* name)
{
    return server->openMutex(this, name);
}

Queue* SessionImpl::createQueue(char const* name)
{
    return server->createQueue(this, name, true);
}

Queue* SessionImpl::openQueue(char const* name)
{
    return server->openQueue(this, name);
}

SharedMemory* SessionImpl::createSharedMemory(char const* name, Object const& objDesc)
{
    return server->createSharedMemory(this, name, objDesc, false);    
}

SharedMemory* SessionImpl::createSharedMemory(char const* name, void const* obj, size_t objSize)
{
    Object objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    return server->createSharedMemory(this, name, objDesc, true);    
}

SharedMemory* SessionImpl::openSharedMemory(char const* name)
{
    return server->openSharedMemory(this, name);    
}

Lock* SessionImpl::createLock(char const* name)
{
    return server->createLock(this, name, true);
}

Lock* SessionImpl::openLock(char const* name)
{
    return server->openLock(this, name);
}

Barrier* SessionImpl::createBarrier(char const* name, int nSessions)
{
    return server->createBarrier(this, name, nSessions, true);
}

Barrier* SessionImpl::openBarrier(char const* name)
{
    return server->openBarrier(this, name);
}
//------------------------------------------------------------------------------

bool SessionImpl::isLocal()
{
    return socket == NULL;
}

bool SessionImpl::isOk()
{
    return socket == NULL ? true : socket->is_ok();
}

void SessionImpl::getErrorText(char* buf, size_t bufSize)
{
    if (socket == NULL) { 
        strncpy(buf, "ok", bufSize);
    } else { 
        socket->get_error_text(buf, bufSize);
    }
}

void SessionImpl::shutdownServer() {}
   
void SessionImpl::showServerInfo(char* buf, size_t) 
{ 
    dump(buf);
}

void SessionImpl::getSessionID(char* buf, size_t) 
{ 
    sprintf(buf,"%p",this);
}

void SessionImpl::readBody(void* buf, size_t size)
{
    if (!socket->read(buf, size)) { 
	RIPC_THROW(IoException);
    }
}

void SessionImpl::skipBody(size_t size)
{
    char buf[4096];
    while (size > 0) { 
	size_t toRead = size < sizeof(buf) ? size : sizeof(buf);
	if (!socket->read(buf, toRead)) { 
	    RIPC_THROW(IoException);
	}
	size -= toRead;
    }
}
