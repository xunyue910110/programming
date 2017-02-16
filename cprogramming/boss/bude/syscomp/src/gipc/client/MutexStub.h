#ifndef __MUTEX_STUB_H__
#define __MUTEX_STUB_H__

#include "Mutex.h"
#include "PrimitiveStub.h"

//客户端互斥类实现
class MutexStub : public PrimitiveStub, public Mutex { 
  public:
    void priorityLock(int rank);    
    bool priorityLock(int rank, unsigned timeout);
    void unlock();

    MutexStub(SessionStub* session, char const* name, Response& resp) 
      : PrimitiveStub(session, name, resp) {}    
    
};

#endif
