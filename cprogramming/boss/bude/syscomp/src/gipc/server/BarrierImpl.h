#ifndef __BARRIER_IMPL_H__
#define __BARRIER_IMPL_H__

#include "PrimitiveImpl.h"
#include "Barrier.h"

class BarrierImpl : public PrimitiveImpl, public Barrier { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  reset();
    char* dump(char* buf);

    BarrierImpl(Server* server, char const* name, int nSessions) 
    : PrimitiveImpl(server, name)
    {
	this->nSessions = nSessions;
	nBlocked = 0;
	sequenceNo = 0;
    }
    void deletePrimitive();

  protected:
    int sequenceNo;
    int nSessions;
    int nBlocked;
};

#endif
