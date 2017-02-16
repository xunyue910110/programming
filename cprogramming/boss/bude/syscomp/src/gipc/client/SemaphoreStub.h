#ifndef __SEMAPHORE_STUB_H__
#define __SEMAPHORE_STUB_H__

#include "Semaphore.h"
#include "PrimitiveStub.h"

class SemaphoreStub : public PrimitiveStub, public Semaphore { 
  public:
    void signal(int count) ;

    SemaphoreStub(SessionStub* session, char const* name, Response& resp) 
      : PrimitiveStub(session, name, resp) {}    
};

#endif
