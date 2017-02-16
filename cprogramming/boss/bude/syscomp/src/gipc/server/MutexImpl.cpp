#define RIPC_IMPLEMENTATION

#include "MutexImpl.h"
#include "WaitObject.h"
#include "LockObject.h"
#include "Server.h"

bool MutexImpl::detectDeadlock(WaitObject* wob, SessionImpl* session) 
{ 
    SessionImpl* owner = this->owner;
    if (owner != NULL) { 
	if (owner == session) { 
	    return true;
	} else if ((wob = owner->waitFor) != NULL) { 
	    return wob->detectDeadlock(session);
	}
    }
    return false;
}


void MutexImpl::priorityLock(int rank) 
{ 
    SessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (owner == NULL || owner == session) {
	if (owner == NULL) { 
	    owner = session;
	    counter = 1;
	    lck = session->addLock(this, 0);
	} else { 
	    counter += 1;
	}
    } else { 
	WaitObject wob(rank, Server::LOCKED_SERVER);
	WaitObject *wp;
	server->lock();
	if ((wp = owner->waitFor) != NULL) { 
	    if (wp->detectDeadlock(session)) { 
		server->unlock();
		RIPC_THROW(DeadlockException);
	    }
	}
	server->waitNotification(wob, this);
    }
}

bool MutexImpl::priorityLock(int rank, unsigned timeout) 
{ 
    SessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (owner == NULL || owner == session) {
	if (owner == NULL) { 
	    owner = session;
	    counter = 1;
	    lck = session->addLock(this, 0);
	} else { 
	    counter += 1;
	}
	return true;
    } else { 
	WaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }
}    

void MutexImpl::unlock() 
{
    SessionImpl* session = server->getSession();
    if (owner != session) { 
	RIPC_THROW(NotOwnerException);
    }
    RIPC_CS(cs);
    if (--counter == 0) { 	
	owner = NULL;
	session->removeLock(lck);	
	lck = NULL;
	if (!wobList.isEmpty()) { 
            WaitObject* wob = wobList.next;
	    wob->sendNotification();
            wob->unlink();
	    owner = wob->session;
	    counter = 1;
	    lck = owner->addLock(this, 0);
	}
    }
}

void MutexImpl::unlock(LockObject*)
{
    RIPC_CS(cs);
    if (lck != NULL) { 
	owner = NULL;
	counter = 0;
	lck->owner->removeLock(lck);	
	lck = NULL;
	if (!wobList.isEmpty()) { 
            WaitObject* wob = wobList.next;
	    wob->sendNotification();
            wob->unlink();
	    owner = wob->session;
	    counter = 1;
	    lck = owner->addLock(this, 0);
	}
    }
}

	
void MutexImpl::priorityWait(int rank)
{
    priorityLock(rank);
}

bool MutexImpl::priorityWait(int rank, unsigned timeout)
{
    return priorityLock(rank, timeout);
}

void MutexImpl::reset()
{
    unlock(lck);
}
   
MutexImpl::MutexImpl(Server* server, char const* name, bool locked)  
: PrimitiveImpl(server, name)
{
    if (locked) { 
	owner = server->getSession();
	counter = 1;
	lck = owner->addLock(this);
    } else { 
	owner = NULL;
	counter = 0;
	lck = NULL;
    }
}


void MutexImpl::deletePrimitive()
{
    server->deleteMutex(this);
}

char* MutexImpl::dump(char* buf)
{
    buf = PrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Owner: %p\n  Nesting: %d\n", owner, counter);
    return buf;
}
