#ifndef __SEMAPHORE_IMPL_H__
#define __SEMAPHORE_IMPL_H__

#include "Semaphore.h"
#include "PrimitiveImpl.h"

class SemaphoreImpl : public PrimitiveImpl, public Semaphore { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  signal(int n);
    void  reset();
    char* dump(char* buf);

    SemaphoreImpl(Server* server, char const* name, int initValue)
    : PrimitiveImpl(server, name)
    {
	count = initValue;
    }
    void deletePrimitive();
    
  protected:
    int count;
};

#endif
