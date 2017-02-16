#define RIPC_IMPLEMENTATION

#include <stdio.h>
#include "PrimitiveImpl.h"
#include "WaitObject.h"
#include "Server.h"

PrimitiveImpl::PrimitiveImpl(Server* server, char const* name) 
{
    this->server = server;
    this->name = (char*)name;
    server->assignId(this);
    accessCount = 0;
}

PrimitiveImpl::~PrimitiveImpl() 
{
    delete[] name;
}

void PrimitiveImpl::beginAccess(SessionImpl* session)
{
    accessCount += 1;
}

void PrimitiveImpl::close()
{
    server->getSession()->endAccess(this);
}

void PrimitiveImpl::endAccess(SessionImpl* session)
{
    RIPC_CS(server->cs);
    if (--accessCount == 0) { 
	deletePrimitive();
    }
}
    
void PrimitiveImpl::unlock(LockObject*) 
{
}

char const* PrimitiveImpl::getName() 
{ 
    return name;
}
    
bool PrimitiveImpl::alreadyExists() 
{
    return server->getSession()->exists;
}


bool PrimitiveImpl::detectDeadlock(WaitObject*, SessionImpl*) 
{ 
    return false;
}

void PrimitiveImpl::addWaitObject(WaitObject* wob) 
{ 
    wob->prim = this;
    WaitObject* head = &wobList;        
    WaitObject* last = head;
    while ((last = last->prev) != head && last->rank > wob->rank);
    wob->linkAfter(last);
}


void PrimitiveImpl::wakeupAll() 
{ 
    WaitObject *head = &wobList, *wob = head; 
    while ((wob = wob->next) != head) { 
        wob->sendNotification();
    }
    head->prune();
}

void PrimitiveImpl::copyObject(Object& dst, Object& src)
{
    if (dst.data == NULL) { 
	dst.data = allocate(src.size);
	memcpy(dst.data, src.data, src.size);
	dst.size = src.size;
	dst.container = this;
	dst.deallocator = &deallocator;
    } else {
	assert(dst.size >= src.size);
	dst.size = src.size;
	memcpy(dst.data, src.data, src.size);
    }
}


char* PrimitiveImpl::dump(char* buf)
{
    buf += sprintf(buf, "%s:%s\n", getTypeName(), name);
    buf += sprintf(buf, "  Access Count: %u\n", accessCount);
    buf += sprintf(buf, "  Blocked Sessions:\n");
    cs.enter();
    WaitObject* head = &wobList, *wob = head; 
    while ((wob = wob->next) != head) { 
	buf += sprintf(buf, "    %p\n", wob->session);
    }
    cs.leave();
    return buf;
}



