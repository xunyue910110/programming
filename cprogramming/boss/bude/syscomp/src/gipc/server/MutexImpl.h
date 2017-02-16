#ifndef __MUTEX_IMPL_H__
#define __MUTEX_IMPL_H__

#include "Mutex.h"
#include "PrimitiveImpl.h"

class MutexImpl : public PrimitiveImpl, public Mutex { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  reset();
    char* dump(char* buf);
    void  priorityLock(int rank);
    bool  priorityLock(int rank, unsigned timeout); 
    void  unlock();

    MutexImpl(Server* server, char const* name, bool locked);  
    void deletePrimitive();

  protected: 
    bool detectDeadlock(WaitObject* wob, SessionImpl* session);
    void unlock(LockObject* lob);
    
    SessionImpl* owner;
    LockObject*  lck;
    int              counter;
};

#endif
