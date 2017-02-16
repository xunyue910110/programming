#define RIPC_IMPLEMENTATION

#include "SharedMemoryImpl.h"
#include "WaitObject.h"
#include "Server.h"

SharedMemoryImpl::SharedMemoryImpl(Server* server, char const* name, 
					   Object const& initObjDesc, bool copy)
: PrimitiveImpl(server, name) 
{
    sequenceNo = 0;
    if (copy) { 
	objDesc.data = allocate(initObjDesc.size);
	memcpy(objDesc.data, initObjDesc.data, initObjDesc.size);
	objDesc.size = initObjDesc.size;
	objDesc.container = this;
	objDesc.deallocator = &deallocator;
    } else { 
	objDesc = initObjDesc;
    }
}


SharedMemoryImpl::SharedMemoryImpl(Server* server, char const* name, 
					   SessionImpl* session, size_t objSize)
: PrimitiveImpl(server, name) 
{
    sequenceNo = 0;
    objDesc.data = allocate(objSize);
    session->readBody(objDesc.data, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
}


void SharedMemoryImpl::priorityWait(int rank)
{ 
    RIPC_CS(cs);
    SessionImpl* session = server->getSession();
    if (session->isLocal() || session->sequenceNo == sequenceNo) { 
	WaitObject wob(rank);
	server->waitNotification(wob, this);
    }	
}

bool SharedMemoryImpl::priorityWait(int rank, unsigned timeout)
{ 
    RIPC_CS(cs);
    SessionImpl* session = server->getSession();
    if (session->isLocal() || session->sequenceNo == sequenceNo) { 
	WaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }	
    return true;
}


void SharedMemoryImpl::get(Object& objDesc)
{
    RIPC_CS(cs);
    SessionImpl* session = server->getSession();
    session->sequenceNo = sequenceNo;
    copyObject(objDesc, this->objDesc);
}
	
void SharedMemoryImpl::set(Object const& objDesc)
{
    RIPC_CS(cs);
    SessionImpl* session = server->getSession();
    wakeupAll();
    session->sequenceNo = ++sequenceNo;
    this->objDesc.deallocate();
    this->objDesc = objDesc;
}

void SharedMemoryImpl::set(void const* obj, size_t objSize) 
{ 
    Object objDesc;
    objDesc.data = allocate(objSize);
    memcpy(objDesc.data, obj, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    set(objDesc);
}

void SharedMemoryImpl::set(SessionImpl* session, size_t size)
{
    Object objDesc;
    objDesc.data = allocate(size);
    session->readBody(objDesc.data, size);
    objDesc.size = size;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    set(objDesc);
}


void SharedMemoryImpl::reset() {}

void SharedMemoryImpl::deletePrimitive()
{
    objDesc.deallocate();
    server->deleteSharedMemory(this);
}

char* SharedMemoryImpl::dump(char* buf)
{
    buf = PrimitiveImpl::dump(buf);
    RIPC_CS(cs);
    buf += sprintf(buf, "  Object: %p\n  Sequence Number: %d\n", objDesc.data, sequenceNo);
    return buf;
}


