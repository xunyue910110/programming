#ifndef __SQUEUE_STUB_H__
#define __SQUEUE_STUB_H__

#include "Queue.h"
#include "PrimitiveStub.h"

class QueueStub : public PrimitiveStub, public Queue 
{ 
  public:
    void priorityGet(int rank, Object& objDesc);
    bool priorityGet(int rank, Object& objDesc, unsigned timeout);
    void put(Object const& objDesc);
    void put(void const* obj, size_t objSize);
    void broadcast(Object const& objDesc);
    void broadcast(void const* obj, size_t objSize);

    QueueStub(SessionStub* session, char const* name, Response& resp) 
      : PrimitiveStub(session, name, resp) {}
};


#endif
