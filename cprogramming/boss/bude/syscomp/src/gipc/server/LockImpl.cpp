#define RIPC_IMPLEMENTATION

#include "LockImpl.h"
#include "WaitObject.h"
#include "LockObject.h"
#include "Server.h"


bool LockImpl::detectDeadlock(WaitObject* wob, SessionImpl* session) 
{ 
    LockObject* writer = this->writer;
    if (writer != NULL) {
	if (writer->owner == session) { 
	    return true;
	} else if ((wob = writer->owner->waitFor) != NULL) { 
	    return wob->detectDeadlock(session);
	}
    } else { 
	WaitObject* rwob;
	for (LockObject* reader = readers; reader != NULL; reader = reader->nextOwner) { 
	    if (reader->owner == session) {
		return true;
	    } else if (reader->owner != wob->session && (rwob = reader->owner->waitFor) != NULL) { 
		if (rwob->detectDeadlock(session)) { 
		    return true;
		}
	    } 
	}
    }		    		    
    return false;
}


void LockImpl::priorityExclusiveLock(int rank)
{ 
    SessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 
	if (writer == NULL 
	    && (readers == NULL || readers->owner == session && readers->nextOwner == NULL))
	{
	    if (readers != NULL) { 
		readers->flags = EXCLUSIVE_LOCK;
		writer = readers;
		readers = NULL;
	    } else { 
		writer = session->addLock(this, EXCLUSIVE_LOCK);
	    }
	} else {
	    server->lock(); 
	    WaitObject* wp;
	    if (writer != NULL) { 
		if ((wp = writer->owner->waitFor) != NULL) { 
		    if (wp->detectDeadlock(session)) {
			server->unlock(); 
			RIPC_THROW(DeadlockException);
		    }
		} 
	    } else {
		for (LockObject* lob = readers; lob != NULL; lob = lob->nextOwner) { 
		    if (lob->owner == session) { 
                        WaitObject* head = &wobList;
                        wp = head;
                        while ((wp = wp->next) != head) { 
			    if ((wp->flags & EXCLUSIVE_LOCK) != 0) { 
				server->unlock(); 
				RIPC_THROW(DeadlockException);
			    }
			}
		    } else if ((wp = lob->owner->waitFor) != NULL) { 
			if (wp->detectDeadlock(session)) {
			    server->unlock(); 
			    RIPC_THROW(DeadlockException);
			}
		    }
		}			
	    }
	    WaitObject wob(rank, Server::LOCKED_SERVER|EXCLUSIVE_LOCK);
	    server->waitNotification(wob, this);
	}
    }
}
    

bool LockImpl::priorityExclusiveLock(int rank, unsigned timeout)
{ 
    SessionImpl* session = server->getSession();
    bool result = true;
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 	    
	if (writer == NULL 
	    && (readers == NULL || readers->owner == session && readers->nextOwner == NULL))
	{
	    if (readers != NULL) { 
		readers->flags = EXCLUSIVE_LOCK;
		writer = readers;
		readers = NULL;
	    } else { 
		writer = session->addLock(this, EXCLUSIVE_LOCK);
	    }
	} else { 
	    WaitObject wob(rank, EXCLUSIVE_LOCK);
	    server->waitNotificationWithTimeout(wob, this, timeout);
	    if (!wob.signaled) {
		result = false;
	    }
	}
    }
    return result;
}
    


void LockImpl::prioritySharedLock(int rank)
{ 
    SessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 	    
	if (writer == NULL) {  	
	    LockObject* lob;
	    for (lob = readers; lob != NULL && lob->owner != session; lob = lob->nextOwner);
	    if (lob == NULL) { 
		lob = session->addLock(this, SHARED_LOCK);
		server->lock();
		lob->nextOwner = readers;
		readers = lob;
		server->unlock();
	    }
	} else { 
	    WaitObject* wp;
	    server->lock(); 
	    if ((wp = writer->owner->waitFor) != NULL) { 
		if (wp->detectDeadlock(session)) { 		    
		    server->unlock();
		    RIPC_THROW(DeadlockException);
		}
	    } 
	    WaitObject wob(rank, Server::LOCKED_SERVER|SHARED_LOCK);
	    server->waitNotification(wob, this);
	}
    }
}
    

bool LockImpl::prioritySharedLock(int rank, unsigned timeout) 
{ 
    SessionImpl* session = server->getSession();
    bool result = true;
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 	    
	if (writer == NULL) {  	
	    LockObject* lob;
	    for (lob = readers; lob != NULL && lob->owner != session; lob = lob->nextOwner);
	    if (lob == NULL) { 
		lob = session->addLock(this, SHARED_LOCK);
		server->lock(); 
		lob->nextOwner = readers;
		readers = lob;
		server->unlock(); 
	    }
	} else { 
	    WaitObject wob(rank, SHARED_LOCK);
	    server->waitNotificationWithTimeout(wob, this, timeout);
	    if (!wob.signaled) {
		result = false;
	    }
	}
    }
    return result;
}
   

void LockImpl::unlock()
{
    unlock(server->getSession());
}

void LockImpl::unlock(LockObject* lob)
{ 
    unlock(lob->owner);
}

void LockImpl::retry()
{ 
    WaitObject* head = &wobList;
    WaitObject* wob = head;
    while ((wob = head->next) != head
           && ((wob->flags & EXCLUSIVE_LOCK) == 0 || readers == NULL 
               || (readers->owner == wob->session && readers->nextOwner == NULL)))
    {
	wob->sendNotification();
        wob->unlink();
	if ((wob->flags & EXCLUSIVE_LOCK) == 0) { 
	    LockObject* lob = wob->session->addLock(this, SHARED_LOCK);
	    server->lock();
	    lob->nextOwner = readers;
	    readers = lob;
	    server->unlock();
	} else { 
	    if (readers != NULL) {
		readers->flags = EXCLUSIVE_LOCK;
		writer = readers;
		readers = NULL;
	    } else {
		writer = wob->session->addLock(this, EXCLUSIVE_LOCK);
	    }
	    break;
	}
    }
}
	


void LockImpl::unlock(SessionImpl* session)
{
    RIPC_CS(cs);
    if (writer != NULL) {
	if (writer->owner != session) { 
	    RIPC_THROW(NotOwnerException);
	}
	session->removeLock(writer);
	writer = NULL;
    } else { 
	LockObject *lob, *prev;
	for (lob=readers, prev=NULL; lob != NULL && lob->owner != session; prev=lob, lob=lob->nextOwner);
	if (lob == NULL) { 
	    RIPC_THROW(NotOwnerException);
	}
	if (prev == NULL) { 
	    readers = lob->nextOwner;
	} else { 
	    prev->nextOwner = lob->nextOwner;
	}
	session->removeLock(lob);
    }
    retry();
}


void LockImpl::reset() 
{
    RIPC_CS(cs);
    if (writer != NULL) {
	writer->owner->removeLock(writer);
	writer = NULL;
    } else { 
	for (LockObject* lob = readers; lob != NULL; lob = lob->nextOwner) { 
	    lob->owner->removeLock(lob);
	}
	readers = NULL;
    }
    retry();
}

	
void LockImpl::priorityWait(int rank)
{
    priorityExclusiveLock(rank);
}

bool LockImpl::priorityWait(int rank, unsigned timeout)
{
    return priorityExclusiveLock(rank, timeout);
}

void LockImpl::deletePrimitive()
{
    server->deleteLock(this);
}

char* LockImpl::dump(char* buf)
{
    buf = PrimitiveImpl::dump(buf);
    RIPC_CS(cs);
    if (writer != NULL) { 
	buf += sprintf(buf, "  Exclusive Lock: %p\n", writer->owner);
    } else if (readers != NULL) { 
	buf += sprintf(buf, "  Shared Locks:\n");
	for (LockObject* lob = readers; lob != NULL; lob = lob->nextOwner) { 
	    buf += sprintf(buf, "    %p\n", lob->owner);
	}
    }
    return buf;
}
