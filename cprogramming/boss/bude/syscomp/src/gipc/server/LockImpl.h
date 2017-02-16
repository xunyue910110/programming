#ifndef __LOCK_IMPL_H__
#define __LOCK_IMPL_H__

#include "Lock.h"
#include "PrimitiveImpl.h"

class LockImpl : public PrimitiveImpl, public Lock { 
  public:
    enum LOCK_TYPES { 
	EXCLUSIVE_LOCK = 1, 
	SHARED_LOCK    = 2
    };


    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  reset();
    char* dump(char* buf);
    void  priorityExclusiveLock(int rank);
    void  prioritySharedLock(int rank);
    bool  priorityExclusiveLock(int rank, unsigned timeout); 
    bool  prioritySharedLock(int rank, unsigned timeout); 
    void  unlock();

    LockImpl(Server* server, char const* name)  
    : PrimitiveImpl(server, name) 
    {
	writer = NULL;
	readers = NULL;
    }
    void deletePrimitive();

  protected: 
    bool detectDeadlock(WaitObject* wob, SessionImpl* session);
    void unlock(LockObject* lob);
    void unlock(SessionImpl* session);
    void retry();
    
    LockObject* writer;
    LockObject* readers;
};

#endif
