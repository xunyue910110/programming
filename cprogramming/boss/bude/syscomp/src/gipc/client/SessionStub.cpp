#define RIPC_IMPLEMENTATION

#include "SessionStub.h"


SessionStub::SessionStub(char const* host, int port)
{ 
    socket = Socket::connect(host, port);
    primitives = NULL;
}


void SessionStub::send(Request& req) 
{
    RIPC_CS(cs);
    buf.packRequest(req);
    socket->write(buf.getAddr(), buf.getSize());	
}

void SessionStub::sendAndReceive(Request& req, Response& resp, Primitive* prim) 
{
    RIPC_CS(cs);
    unsigned bufSize = resp.objDesc.size;
    buf.packRequest(req);
    socket->write(buf.getAddr(), buf.getSize());	
    socket->read(buf.getAddr(), sizeof(ResponseHeader)); //read header 
    buf.unpackResponse(resp);	
    if (resp.statusCode == Response::OK && resp.objDesc.size != 0) { 
	if (resp.objDesc.data != NULL) { 
	    assert(bufSize >= resp.objDesc.size);
	} else { 
	    resp.objDesc.data = prim->allocate(resp.objDesc.size);	
	    resp.objDesc.deallocator = &Primitive::deallocator;
	    resp.objDesc.container = prim;
	}
	socket->read(resp.objDesc.data, resp.objDesc.size);
    }
}

void SessionStub::sendAndVerify(Request& req, Response& resp, Primitive* prim) 
{
    sendAndReceive(req, resp, prim);
    switch (resp.statusCode) { 
      case Response::OK:
      case Response::ALREADY_EXISTS:
      case Response::TIMEOUT_EXPIRED:
	return;
      case Response::NOT_FOUND:
	RIPC_THROW(NotFoundException);  
      case Response::NOT_OWNER:
	RIPC_THROW(NotOwnerException);  
      case Response::INVALID_PARAMETER:
	RIPC_THROW(InvalidParameterException);
      case Response::INTERRUPTED:
	RIPC_THROW(InterruptedException);
      case Response::DEADLOCK:
	RIPC_THROW(DeadlockException);
      default:
	RIPC_THROW(InternalException);
    }
}
   
bool SessionStub::sendAndCheckForOk(Request& req, Response& resp, Primitive* prim)
{
    sendAndVerify(req, resp, prim);
    return resp.statusCode == Response::OK;
}
	

Semaphore* SessionStub::createSemaphore(char const* name, int initCount) 
{ 
    Request req;
    Response resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = Request::CREATE_SEMAPHORE;
    req.value = initCount;
    sendAndVerify(req, resp);
    return new SemaphoreStub(this, name, resp);
}
	
Semaphore* SessionStub::openSemaphore(char const* name) 
{
    Request req;
    Response resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = Request::OPEN_SEMAPHORE;
    sendAndVerify(req, resp);
    return resp.statusCode == Response::OK ? new SemaphoreStub(this, name, resp) : NULL;
}
   

Mutex* SessionStub::createMutex(char const* name, bool locked) 
{ 
    Request req;
    Response resp;
    req.opCode = Request::CREATE_MUTEX;
    req.name = (char*)name;
    req.objectId = 0;
    req.value = locked ? 1 : 0;
    sendAndVerify(req, resp);
    return new MutexStub(this, name, resp);
}
	
	
Mutex* SessionStub::openMutex(char const* name) 
{
    Request req;
    Response resp;
    req.opCode = Request::OPEN_MUTEX;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return resp.statusCode == Response::OK ? new MutexStub(this, name, resp) : NULL;
}
   	
    
Queue* SessionStub::createQueue(char const* name) 
{ 
    Request req;
    Response resp;
    req.opCode = Request::CREATE_QUEUE;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return new QueueStub(this, name, resp);
}
	
Queue* SessionStub::openQueue(char const* name) 
{ 
    Request req;
    Response resp;
    req.opCode = Request::OPEN_QUEUE;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return resp.statusCode == Response::OK ? new QueueStub(this, name, resp) : NULL;
}

SharedMemory* SessionStub::createSharedMemory(char const* name, Object const& objDesc)
{ 
    Request req;
    Response resp;
    if (objDesc.data == NULL || objDesc.size <= 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    req.opCode = Request::CREATE_SHMEM;
    req.name = (char*)name;
    req.objectId = 0;
    req.objDesc = objDesc;
    sendAndVerify(req, resp);
    return new SharedMemoryStub(this, name, resp);
}
	
SharedMemory* SessionStub::createSharedMemory(char const* name, void const* obj, size_t objSize)
{ 
    Object objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    return createSharedMemory(name, objDesc);
}
	

SharedMemory* SessionStub::openSharedMemory(char const* name) 
{
    Request req;
    Response resp;
    req.opCode = Request::OPEN_SHMEM;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return resp.statusCode == Response::OK ? new SharedMemoryStub(this, name, resp) : NULL;
}

Lock* SessionStub::createLock(char const* name) 
{ 
    Request req;
    Response resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = Request::CREATE_LOCK;
    sendAndVerify(req, resp);
    return new LockStub(this, name, resp);
}
	
Lock* SessionStub::openLock(char const* name) 
{
    Request req;
    Response resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = Request::OPEN_LOCK;
    sendAndVerify(req, resp);
    return resp.statusCode == Response::OK ? new LockStub(this, name, resp) : NULL;
}

Barrier* SessionStub::createBarrier(char const* name, int nSessions)
{
    if (nSessions <= 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    Request req;
    Response resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = Request::CREATE_BARRIER;
    req.value = nSessions;
    sendAndVerify(req, resp);
    return new BarrierStub(this, name, resp);
}
	
Barrier* SessionStub::openBarrier(char const* name)
{
    Request req;
    Response resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = Request::OPEN_BARRIER;
    sendAndVerify(req, resp);
    return resp.statusCode == Response::OK ? new BarrierStub(this, name, resp) : NULL;
}


void SessionStub::close() 
{
    if (socket != NULL) { 
	Request req;
	req.opCode = Request::CLOSE_SESSION;
        req.objectId = 0;
	send(req);        
	delete socket;
	socket = NULL;
    }
    PrimitiveStub *prim, *next; 
    for (prim = primitives; prim != NULL; prim = next) { 
	next = prim->next;
	delete prim;
    }
    primitives = NULL;
}
   	
void SessionStub::shutdownServer() 
{
    Request req;
    req.opCode = Request::SHUTDOWN;
    req.objectId = 0;
    send(req);	    	
    delete socket;
    socket = NULL;
}

void SessionStub::showServerInfo(char* buf, size_t bufSize) 
{
    if (buf == NULL || bufSize <= 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    Request req;
    Response resp;
    req.opCode = Request::SHOW_INFO;
    req.objectId = 0;
    resp.objDesc.data = buf;
    resp.objDesc.size = bufSize;
    sendAndVerify(req, resp);
}

void SessionStub::getSessionID(char* buf, size_t bufSize) 
{
    if (buf == NULL || bufSize <= 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    Request req;
    Response resp;
    req.opCode = Request::SESSION_ID;
    req.objectId = 0;
    resp.objDesc.data = buf;
    resp.objDesc.size = bufSize;
    sendAndVerify(req, resp);
}


bool SessionStub::isLocal() { 
    return false;
}

bool SessionStub::isOk()
{
    return socket == NULL ? false : socket->is_ok();
}

void SessionStub::getErrorText(char* buf, size_t bufSize)
{
    if (socket == NULL) { 
        strncpy(buf, "socket is not opened", bufSize);
    } else { 
        socket->get_error_text(buf, bufSize);
    }
}


SessionStub::~SessionStub()
{
    close();
}
