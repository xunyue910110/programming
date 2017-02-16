#ifndef __BARRIER_STUB_H__
#define __BARRIER_STUB_H__

#include "Barrier.h"
#include "PrimitiveStub.h"

class BarrierStub : public PrimitiveStub, public Barrier { 
  public:
    BarrierStub(SessionStub* session, char const* name, Response& resp) 
      : PrimitiveStub(session, name, resp) {}
};

#endif
