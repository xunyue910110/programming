#ifndef __LOCK_OBJECT_H__
#define __LOCK_OBJECT_H__

#include "PrimitiveImpl.h"

class LockObject { 
  public:
    LockObject(SessionImpl* session) 
    { 
	owner = session;
    }

    void unlock() 
    { 
	prim->unlock(this);
    }

    SessionImpl*   owner;
    PrimitiveImpl* prim;
    LockObject*    nextLock;
    LockObject*    nextOwner;
    int            flags;
};

#endif
