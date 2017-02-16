#ifndef __LOCK_STUB_H__
#define __LOCK_STUB_H__

#include "Lock.h"
#include "PrimitiveStub.h"

//客户端锁实现
class LockStub : public PrimitiveStub, public Lock { 
  public:
    void priorityExclusiveLock(int rank);
    bool priorityExclusiveLock(int rank, unsigned timeout);
    void prioritySharedLock(int rank);
    bool prioritySharedLock(int rank, unsigned timeout);
    void unlock();

    LockStub(SessionStub* session, char const* name, Response& resp) 
      : PrimitiveStub(session, name, resp) {}
};

#endif




